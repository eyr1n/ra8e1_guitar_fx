#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "r_iic_master.h"

class TwoWire {
public:
  void begin(i2c_master_ctrl_t *i2c_ctrl, const i2c_master_cfg_t *i2c_cfg) {
    i2c_ctrl_ = i2c_ctrl;

    fsp_err_t err;
    err = R_IIC_MASTER_Open(i2c_ctrl_, i2c_cfg);
    while (err)
      ;
    err = R_IIC_MASTER_CallbackSet(i2c_ctrl_, i2c_callback, this, nullptr);
    while (err)
      ;
  }

  void beginTransmission(uint8_t address) {
    fsp_err_t err;
    err = R_IIC_MASTER_SlaveAddressSet(i2c_ctrl_, address,
                                       I2C_MASTER_ADDR_MODE_7BIT);
    while (err)
      ;
    buf_idx_ = 0;
    i2c_event_ = static_cast<i2c_master_event_t>(0);
  }

  uint8_t endTransmission() {
    fsp_err_t err;
    err = R_IIC_MASTER_Write(i2c_ctrl_, buf_.data(), buf_idx_, false);
    while (err)
      ;

    while (true) {
      switch (i2c_event_) {
      case I2C_MASTER_EVENT_TX_COMPLETE:
        return 0;
      case I2C_MASTER_EVENT_ABORTED:
        return 4;
      default:
        break;
      }
    }
  }

  size_t write(uint8_t data) {
    buf_[buf_idx_++] = data;
    return 1;
  }

private:
  i2c_master_ctrl_t *i2c_ctrl_;
  volatile i2c_master_event_t i2c_event_;

  std::array<uint8_t, 255> buf_;
  size_t buf_idx_;

  static inline void i2c_callback(i2c_master_callback_args_t *p_args) {
    static_cast<TwoWire *>(p_args->p_context)->i2c_event_ = p_args->event;
  }
};

inline TwoWire Wire;
