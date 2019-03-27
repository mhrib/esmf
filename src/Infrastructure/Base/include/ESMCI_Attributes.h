// $Id$
//
// Earth System Modeling Framework
// Copyright 2002-2018, University Corporation for Atmospheric Research,
// Massachusetts Institute of Technology, Geophysical Fluid Dynamics
// Laboratory, University of Michigan, National Centers for Environmental
// Prediction, Los Alamos National Laboratory, Argonne National Laboratory,
// NASA Goddard Space Flight Center.
// Licensed under the University of Illinois-NCSA License.
//
// ESMF Attributes C++ include file
//
//-----------------------------------------------------------------------------
//

#ifndef ESMCI_ATTRIBUTES_H
#define ESMCI_ATTRIBUTES_H

//-----------------------------------------------------------------------------

#include <vector>
#include <fstream>

#include "ESMCI_Util.h"
#include "json.hpp"

using json = nlohmann::json;  // Convenience rename for JSON namespace.

//tdk:LAST: remove all tdk stuff (search for tdk)

// Standard ESMF check error macros
#define ESMF_CHECKERR_STD(name_rc, actual_rc, msg, update_rc) {if (actual_rc != ESMF_SUCCESS) {ESMCI::esmf_attrs_error local_macro_error(name_rc, actual_rc, msg); if (ESMC_LogDefault.MsgFoundError(actual_rc, local_macro_error.what(), ESMC_CONTEXT, &update_rc)) throw(local_macro_error);}}

#define ESMF_THROW_JSON(json_exc, name_rc, actual_rc, update_rc) {ESMC_LogDefault.MsgFoundError(actual_rc, json_exc.what(), ESMC_CONTEXT, &update_rc); throw(ESMCI::esmf_attrs_error(name_rc, actual_rc, json_exc.what()));}

#define ESMF_CATCH_PASSTHRU(exc_esmf) {ESMC_LogDefault.MsgFoundError(exc_esmf.getReturnCode(), exc_esmf.what(), ESMC_CONTEXT, nullptr); throw(exc_esmf);}

#define ESMF_CATCH_ISOC catch (ESMCI::esmf_attrs_error &exc_esmf) {ESMC_LogDefault.MsgFoundError(exc_esmf.getReturnCode(), exc_esmf.what(), ESMC_CONTEXT, nullptr); rc = exc_esmf.getReturnCode();} catch(...) {std::string msg;if (rc == ESMF_SUCCESS) {msg = "Unhandled throw and return code is ESMF_SUCCESS(?). Changing return code to ESMF_FAILURE";rc = ESMF_FAILURE;} else {msg = "Unhandled throw";}ESMC_LogDefault.MsgFoundError(rc, msg, ESMC_CONTEXT, nullptr);}

#define ESMF_CATCH_ATTRS catch (json::out_of_range &exc_json) {ESMF_THROW_JSON(exc_json, "ESMC_RC_NOT_FOUND", ESMC_RC_NOT_FOUND, rc);} catch (json::type_error &exc_json) {ESMF_THROW_JSON(exc_json, "ESMC_RC_ARG_BAD", ESMC_RC_ARG_BAD, rc);} catch (ESMCI::esmf_attrs_error &exc_esmf) {ESMF_CATCH_PASSTHRU(exc_esmf);} catch (...) {ESMF_CHECKERR_STD("", ESMF_FAILURE, "Unhandled throw", rc);}

#define ESMF_CATCH_JSON catch (json::out_of_range &e) {ESMF_THROW_JSON(e, "ESMC_RC_NOT_FOUND", ESMC_RC_NOT_FOUND, rc);} catch (json::type_error &e) {ESMF_THROW_JSON(e, "ESMC_RC_ARG_BAD", ESMC_RC_ARG_BAD, rc);}

//-----------------------------------------------------------------------------
//BOP
// !CLASS:  Attributes
//
// !DESCRIPTION:
// The code in this file implements the Attributes defined type
// and methods.
//
//-----------------------------------------------------------------------------
//
// !USES:

namespace ESMCI {

typedef const std::string key_t;
typedef const std::vector<json>* const vecjson_t;
typedef const json::array_t* const arrjson_t;

enum ESMC_ISOCType {C_INT, C_LONG, C_FLOAT, C_DOUBLE, C_CHAR};

//-----------------------------------------------------------------------------

class esmf_attrs_error : public std::exception
{
public:
  esmf_attrs_error(key_t &code_name, int rc, key_t &msg);

  int getReturnCode() {return this->rc;}

  const char* what() const noexcept {return this->msg.c_str();}

private:
  std::string msg;
  int rc;
};

//-----------------------------------------------------------------------------

class Attributes {

protected:
  json storage;  // JSON object store for keys/values managed by this instance

  static json::json_pointer formatKey(key_t& key, int& rc);
  virtual void init(void) {this->storage = json::object();}

public:
  Attributes(void) {this->init();}
//  Attributes(void) = default;  // Default constructor
  virtual ~Attributes(void) = default; // Default destructor
  Attributes(Attributes&&) = delete; // Move constructor
  Attributes(const Attributes&) = delete; // Copy constructor
  Attributes&operator=(const Attributes&) = delete; // Copy assignment
  Attributes&operator=(Attributes&&) = delete; // Move assignment

  Attributes(const json& storage); // Copy constructor from JSON
  Attributes(json&& storage); // Move constructor from JSON
  Attributes(key_t& input, int& rc); // Parse JSON string constructor

  std::string dump(int& rc) const;
  std::string dump(int indent, int& rc) const;

  void erase(key_t& key, key_t& keyChild, int& rc);

  template <typename T>
  T get(key_t &key, int &rc, T *def = nullptr, int *index = nullptr) const;

  //tdk:remove this interface
  void get_isoc(ESMCI::ESMC_ISOCType ictype, void *ret, char* key, int& rc,
    void* def = nullptr) const;

  const json& getStorageRef(void) const;

  json& getStorageRefWritable(void);

  template <typename T, typename JT>
  T getPointer(key_t &key, int &rc) const;

  bool hasKey(key_t &key, int &rc, bool isptr = false) const;

  bool isSetNull(key_t &key, int &rc) const;

  void parse(key_t &input, int &rc);

  void deserialize(char *buffer, int *offset, int& rc);

  void serialize(char *buffer, int *length, int *offset,
    ESMC_InquireFlag inquireflag, int &rc);

  void set(key_t &key, const ESMCI::Attributes &attrs, bool force, int &rc);
  void set(key_t &key, bool force, int &rc);  // set null

  template <typename T>
  void set(key_t& key, T value, bool force, int& rc, int *index = nullptr);

  template <typename T>
  void set(key_t& key, T *values, int count, bool force, int& rc);

  void update(const Attributes& attrs, int& rc);

};

//-----------------------------------------------------------------------------

class PackageFactory
// tdk: FEATURE: add rule of five
{
private:
  json cache = json::object();
  json uris = json::object();

public:
  PackageFactory(void) = default;  // Default constructor
  ~PackageFactory(void) = default; // Default destructor

  json getOrCreateJSON(key_t& key, int& rc, key_t& uri = "");
};

//-----------------------------------------------------------------------------

void alignOffset(int &offset);
void broadcastAttributes(ESMCI::Attributes* attrs, int rootPet, int& rc); // tdk: FEATURE: non-blocking
bool isIn(key_t& target, const std::vector<std::string>& container);
bool isIn(const std::vector<std::string>& target, const std::vector<std::string>& container);
bool isIn(key_t& target, const json& j);

//-----------------------------------------------------------------------------

} // namespace

#endif  // ifdef barrier
