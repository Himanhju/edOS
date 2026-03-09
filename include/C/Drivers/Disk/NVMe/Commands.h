#ifndef NVME_CMDS
#define NVME_CMDS

// ========== ADMIN COMMANDS ==========

enum NVME_ADMIN_COMMANDS{

// IO Submission Queue Commands
NVME_ADMIN_DELETE_IO_SUB_QUEUE=             0x00,
NVME_ADMIN_CREATE_IO_SUB_QUEUE=             0x01,

NVME_ADMIN_GET_LOG_PAGE=                    0x02,

// IO Completion Queue Commands
NVME_ADMIN_DELETE_IO_COM_QUEUE=             0x04,
NVME_ADMIN_CREATE_IO_COM_QUEUE=             0x05,

NVME_ADMIN_IDENTIFY=                        0x06,

NVME_ADMIN_ABORT=                           0x08,

// Feature Commands
NVME_ADMIN_SET_FEATURES=                    0x09,
NVME_ADMIN_GET_FEATURES=                    0x0A,

NVME_ADMIN_ASYNC_EVENT_REQ=                 0x0C,

NVME_ADMIN_NAMESPACE_MANAGEMENT=            0x0D,

// Firmware Commands (be careful)
NVME_ADMIN_FIRMWARE_COMMIT=                 0x10,
NVME_ADMIN_FIRMWARE_IMAGE_DOWNLOAD=         0x11,

NVME_ADMIN_SELF_TEST=                       0x14,

NVME_ADMIN_NAMESPACE_ATTACHMENT=            0x15,

NVME_ADMIN_KEEP_ALIVE=                      0x18,

// Directive commands
NVME_ADMIN_SEND_DIRECTIVE=                  0x19,
NVME_ADMIN_RECEIVE_DIRECTIVE=               0x1A,

NVME_ADMIN_VIRTUAL_MANAGEMENT=              0x1C,

// NVME-MI commands
NVME_ADMIN_NVME_MI_SEND=                    0x1D,
NVME_ADMIN_NVME_MI_RECEIVE=                 0x1E,

NVME_ADMIN_DOORBELL_BUFFER_CONFIG=          0x7C,

NVME_ADMIN_FORMAT_NVM=                      0x80,

// Security commands
NVME_ADMIN_SECURITY_SEND=                   0x81,
NVME_ADMIN_SECURITY_RECEIVE=                0x82,

NVME_ADMIN_SANITIZE=                        0x84

};

// ====================================

// =========== IO COMMANDS ===========

enum NVME_IO_COMMANDS{

NVME_IO_FLUSH=                              0x00,    

// Read/Write Operations
NVME_IO_WRITE=                              0x01,
NVME_IO_READ=                               0x02,

NVME_IO_WRITE_UNCORRECTABLE=                0x04,

NVME_IO_COMPARE=                            0x05,

NVME_IO_WRITE_ZEROS=                        0x08,

NVME_DATASET_MANAGEMENT=                    0x09,

// Reservation commands
NVME_IO_RESERVATION_REGISTER=               0x0D,
NVME_IO_RESERVATION_REPORT=                 0x0E,
NVME_IO_RESERVATION_ACQUIRE=                0x11,
NVME_IO_RESERVATION_RELEASE=                0x15

};

// ===================================

#endif