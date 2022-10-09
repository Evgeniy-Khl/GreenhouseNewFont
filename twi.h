// A START condition has been transmitted
#define START_OK 0x08
// A repeated START condition has been transmitted
#define REPIT_START_OK 0x10
// SLA+W has been transmitted; ACK has been received
#define MT_SLA_ACK 0x18
// SLA+W has been transmitted; NOT ACK has been received
#define MT_SLA_NOT_ACK 0x20
// Data byte has been transmitted; ACK has been received
#define MT_DATA_ACK 0x28
// Data byte has been transmitted; NOT ACK has been received
#define MT_DATA_NOT_ACK 0x30
// Arbitration lost in SLA+W or data bytes
#define ARBITRATION 0x38
// SLA+R has been transmitted; ACK has been received
#define MR_SLA_ACK 0x40
// SLA+R has been transmitted; NOT ACK has been received
#define MR_SLA_NOT_ACK 0x48
// Data byte has been received; ACK has been returned
#define MR_DATA_ACK 0x50
// Data byte has been received; NOT ACK has been returned
#define MR_DATA_NOT_ACK 0x58