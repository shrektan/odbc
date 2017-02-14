#pragma once

#include "nanodbc.h"
#include <Rcpp.h>
#include "tz.h"


namespace odbc {
class odbc_result;

class odbc_connection {
  public:
    odbc_connection(std::string connection_string, std::string time_zone = "") :
      current_result_(nullptr)
      {
        if (time_zone.size() > 0) {
          time_zone_ = date::locate_zone(time_zone);
        } else {
          time_zone_ = date::current_zone();
        }
        c_ = std::make_shared<nanodbc::connection>(connection_string);
      }

    std::shared_ptr<nanodbc::connection> connection() const {
      return std::shared_ptr<nanodbc::connection>(c_);
    }

    void begin() {
      if (t_) {
        Rcpp::stop("Double begin");
      }
      t_ = std::unique_ptr<nanodbc::transaction>(new nanodbc::transaction(*c_));
    }
    void commit() const {
      if (!t_) {
        Rcpp::stop("Commit without beginning transaction");
      }
      t_->commit();
    }
    void rollback() const {
      if (!t_) {
        Rcpp::stop("Rollback without beginning transaction");
      }
      t_->rollback();
    }
    bool has_active_result() const {
      return current_result_ != nullptr;
    }
    bool is_current_result(odbc_result* result) const {
      return current_result_ == result;
    }
    void set_current_result(odbc_result *r);

    const date::time_zone* time_zone() const {
      return time_zone_;
    }

  private:
      std::shared_ptr<nanodbc::connection> c_;
      std::unique_ptr<nanodbc::transaction> t_;
      odbc_result* current_result_;
      const date::time_zone* time_zone_;
};

}
