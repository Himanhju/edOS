#ifndef SATA_DATA_TYPES
#define SATA_DATA_TYPES

#include <Types/Common.h>

typedef enum
{
    FIS_TYPE_REG_H2D    = 0x27,    // Register FIS - host to device
    FIS_TYPE_REG_D2H    = 0x34,    // Register FIS - device to host
    FIS_TYPE_DMA_ACT    = 0x39,    // DMA activate FIS - device to host
    FIS_TYPE_DMA_SETUP    = 0x41,    // DMA setup FIS - bidirectional
    FIS_TYPE_DATA        = 0x46,    // Data FIS - bidirectional
    FIS_TYPE_BIST        = 0x58,    // BIST activate FIS - bidirectional
    FIS_TYPE_PIO_SETUP    = 0x5F,    // PIO setup FIS - device to host
    FIS_TYPE_DEV_BITS    = 0xA1,    // Set device bits FIS - device to host
} FIS_TYPE;

#define GHC_AHCI_ENABLE 0x80000000

typedef struct tagFIS_REG_H2D // Host to Device
{
    // DWORD 0
    byte  fis_type;    // FIS_TYPE_REG_H2D

    byte  pmport:4;    // Port multiplier
    byte  rsv0:3;        // Reserved
    byte  c:1;        // 1: Command, 0: Control

    byte  command;    // Command register
    byte  featurel;    // Feature register, 7:0
    
    // DWORD 1
    byte  lba0;        // LBA low register, 7:0
    byte  lba1;        // LBA mid register, 15:8
    byte  lba2;        // LBA high register, 23:16
    byte  device;        // Device register

    // DWORD 2
    byte  lba3;        // LBA register, 31:24
    byte  lba4;        // LBA register, 39:32
    byte  lba5;        // LBA register, 47:40
    byte  featureh;    // Feature register, 15:8

    // DWORD 3
    byte  countl;    // Count register, 7:0
    byte  counth;    // Count register, 15:8
    byte  icc;        // Isochronous command completion
    byte  control;    // Control register

    // DWORD 4
    byte  rsv1[4];    // Reserved
} FIS_REG_H2D;

typedef struct tagFIS_REG_D2H // Device to Host
{
    // DWORD 0
    byte  fis_type;    // FIS_TYPE_REG_D2H

    byte  pmport:4;    // Port multiplier
    byte  rsv0:2;      // Reserved
    byte  i:1;         // Interrupt bit
    byte  rsv1:1;      // Reserved

    byte  status;      // Status register
    byte  error;       // Error register
    
    // DWORD 1
    byte  lba0;        // LBA low register, 7:0
    byte  lba1;        // LBA mid register, 15:8
    byte  lba2;        // LBA high register, 23:16
    byte  device;      // Device register

    // DWORD 2
    byte  lba3;        // LBA register, 31:24
    byte  lba4;        // LBA register, 39:32
    byte  lba5;        // LBA register, 47:40
    byte  rsv2;        // Reserved

    // DWORD 3
    byte  countl;      // Count register, 7:0
    byte  counth;      // Count register, 15:8
    byte  rsv3[2];     // Reserved

    // DWORD 4
    byte  rsv4[4];     // Reserved
} FIS_REG_D2H;

typedef struct tagFIS_DATA // Bidirectional
{
    // DWORD 0
    byte  fis_type;    // FIS_TYPE_DATA

    byte  pmport:4;    // Port multiplier
    byte  rsv0:4;        // Reserved

    byte  rsv1[2];    // Reserved

    // DWORD 1 ~ N
    dword data[1];    // Payload
} FIS_DATA;

typedef struct tagFIS_PIO_SETUP // Device to Host
{
    // DWORD 0
    byte  fis_type;    // FIS_TYPE_PIO_SETUP

    byte  pmport:4;    // Port multiplier
    byte  rsv0:1;        // Reserved
    byte  d:1;        // Data transfer direction, 1 - device to host
    byte  i:1;        // Interrupt bit
    byte  rsv1:1;

    byte  status;        // Status register
    byte  error;        // Error register

    // DWORD 1
    byte  lba0;        // LBA low register, 7:0
    byte  lba1;        // LBA mid register, 15:8
    byte  lba2;        // LBA high register, 23:16
    byte  device;        // Device register

    // DWORD 2
    byte  lba3;        // LBA register, 31:24
    byte  lba4;        // LBA register, 39:32
    byte  lba5;        // LBA register, 47:40
    byte  rsv2;        // Reserved

    // DWORD 3
    byte  countl;        // Count register, 7:0
    byte  counth;        // Count register, 15:8
    byte  rsv3;        // Reserved
    byte  e_status;    // New value of status register

    // DWORD 4
    word tc;        // Transfer count
    byte  rsv4[2];    // Reserved
} FIS_PIO_SETUP;

typedef struct tagFIS_DMA_SETUP // Device to Host
{
    // DWORD 0
    byte  fis_type;    // FIS_TYPE_DMA_SETUP

    byte  pmport:4;    // Port multiplier
    byte  rsv0:1;        // Reserved
    byte  d:1;        // Data transfer direction, 1 - device to host
    byte  i:1;        // Interrupt bit
    byte  a:1;            // Auto-activate. Specifies if DMA Activate FIS is needed

    byte  rsved[2];       // Reserved

    //DWORD 1&2

    qword DMAbufferID;    // DMA Buffer Identifier. Used to Identify DMA buffer in host memory.
                             // SATA Spec says host specific and not in Spec. Trying AHCI spec might work.

    //DWORD 3
    dword rsvd;           //More reserved

    //DWORD 4
    dword DMAbufOffset;   //Byte offset into buffer. First 2 bits must be 0

    //DWORD 5
    dword TransferCount;  //Number of bytes to transfer. Bit 0 must be 0

    //DWORD 6
    dword resvd;          //Reserved
        
} FIS_DMA_SETUP;

typedef struct tagHBA_PRDT_ENTRY
{
    void *dba;            // Data base address
    dword dbau;            // Data base address upper 32 bits
    dword rsv0;            // Reserved

    // DW3
    dword dbc:22;        // Byte count, 4M max
    dword rsv1:9;        // Reserved
    dword i:1;            // Interrupt on completion
} HBA_PRDT_ENTRY;


#define SATA_PRDT_LEN 255

typedef volatile struct tagHBA_CMD_TBL
{
    // 0x00
    byte  cfis[64];    // Command FIS

    // 0x40
    byte  acmd[16];    // ATAPI command, 12 or 16 bytes

    // 0x50
    byte  rsv[48];    // Reserved

    // 0x80
    HBA_PRDT_ENTRY prdt_entry[SATA_PRDT_LEN];    // Physical region descriptor table entries, 0 ~ 65535
} HBA_CMD_TBL;

typedef volatile struct tagHBA_CMD_HEADER
{
    // DW0
    byte  cfl:5;        // Command FIS length in DWORDS, 2 ~ 16
    byte  a:1;        // ATAPI
    byte  w:1;        // Write, 1: H2D, 0: D2H
    byte  p:1;        // Prefetchable

    byte  r:1;        // Reset
    byte  b:1;        // BIST
    byte  c:1;        // Clear busy upon R_OK
    byte  rsv0:1;        // Reserved
    byte  pmp:4;        // Port multiplier port

    word prdtl;        // Physical region descriptor table length in entries

    // DW1
    volatile
    dword prdbc;        // Physical region descriptor byte count transferred

    // DW2, 3
    HBA_CMD_TBL *ctba;    // Command table descriptor base address
    dword ctbau;        // Command table descriptor base address upper 32 bits

    // DW4 - 7
    dword rsv1[4];    // Reserved
} HBA_CMD_HEADER;

typedef volatile struct tagHBA_FIS
{
    // 0x00
    FIS_DMA_SETUP    dsfis;        // DMA Setup FIS
    byte         pad0[4];

    // 0x20
    FIS_PIO_SETUP    psfis;        // PIO Setup FIS
    byte         pad1[12];

    // 0x40
    FIS_REG_D2H    rfis;        // Register – Device to Host FIS
    byte         pad2[4];

    // 0x58
    word    sdbfis;        // Set Device Bit FIS
    
    // 0x60
    byte         ufis[64];

    // 0xA0
    byte       rsv[0x100-0xA0];
} HBA_FIS;

typedef volatile struct tagHBA_PORT
{
    HBA_CMD_HEADER *clb;        // 0x00, command list base address, 1K-byte aligned
    dword clbu;        // 0x04, command list base address upper 32 bits
    HBA_FIS* fb;        // 0x08, FIS base address, 256-byte aligned
    dword fbu;        // 0x0C, FIS base address upper 32 bits
    dword is;        // 0x10, interrupt status
    dword ie;        // 0x14, interrupt enable
    dword cmd;        // 0x18, command and status
    dword rsv0;        // 0x1C, Reserved
    dword tfd;        // 0x20, task file data
    dword sig;        // 0x24, signature
    dword ssts;        // 0x28, SATA status (SCR0:SStatus)
    dword sctl;        // 0x2C, SATA control (SCR2:SControl)
    dword serr;        // 0x30, SATA error (SCR1:SError)
    dword sact;        // 0x34, SATA active (SCR3:SActive)
    dword ci;        // 0x38, command issue
    dword sntf;        // 0x3C, SATA notification (SCR4:SNotification)
    dword fbs;        // 0x40, FIS-based switch control
    dword rsv1[11];    // 0x44 ~ 0x6F, Reserved
    dword vendor[4];    // 0x70 ~ 0x7F, vendor specific
} HBA_PORT;

typedef volatile struct tagHBA_MEM
{
    // 0x00 - 0x2B, Generic Host Control
    dword cap;        // 0x00, Host capability
    dword ghc;        // 0x04, Global host control
    dword is;        // 0x08, Interrupt status
    dword pi;        // 0x0C, Port implemented
    dword vs;        // 0x10, Version
    dword ccc_ctl;    // 0x14, Command completion coalescing control
    dword ccc_pts;    // 0x18, Command completion coalescing ports
    dword em_loc;    // 0x1C, Enclosure management location
    dword em_ctl;    // 0x20, Enclosure management control
    dword cap2;        // 0x24, Host capabilities extended
    dword bohc;        // 0x28, BIOS/OS handoff control and status

    // 0x2C - 0x9F, Reserved
    byte  rsv[0xA0-0x2C];

    // 0xA0 - 0xFF, Vendor specific registers
    byte  vendor[0x100-0xA0];

    // 0x100 - 0x10FF, Port control registers
    HBA_PORT    ports[32];    // 1 ~ 32
} HBA_MEM;

typedef struct _IDENTIFY_DEVICE_DATA {
  struct {
    word Reserved1 : 1;
    word Retired3 : 1;
    word ResponseIncomplete : 1;
    word Retired2 : 3;
    word FixedDevice : 1;
    word RemovableMedia : 1;
    word Retired1 : 7;
    word DeviceType : 1;
  } GeneralConfiguration;
  word NumCylinders;
  word SpecificConfiguration;
  word NumHeads;
  word Retired1[2];
  word NumSectorsPerTrack;
  word VendorUnique1[3];
  byte  SerialNumber[20];
  word Retired2[2];
  word Obsolete1;
  byte  FirmwareRevision[8];
  byte  ModelNumber[40];
  byte  MaximumBlockTransfer;
  byte  VendorUnique2;
  struct {
    word FeatureSupported : 1;
    word Reserved : 15;
  } TrustedComputing;
  struct {
    byte  CurrentLongPhysicalSectorAlignment : 2;
    byte  ReservedByte49 : 6;
    byte  DmaSupported : 1;
    byte  LbaSupported : 1;
    byte  IordyDisable : 1;
    byte  IordySupported : 1;
    byte  Reserved1 : 1;
    byte  StandybyTimerSupport : 1;
    byte  Reserved2 : 2;
    word ReservedWord50;
  } Capabilities;
  word ObsoleteWords51[2];
  word TranslationFieldsValid : 3;
  word Reserved3 : 5;
  word FreeFallControlSensitivity : 8;
  word NumberOfCurrentCylinders;
  word NumberOfCurrentHeads;
  word CurrentSectorsPerTrack;
  dword  CurrentSectorCapacity;
  byte  CurrentMultiSectorSetting;
  byte  MultiSectorSettingValid : 1;
  byte  ReservedByte59 : 3;
  byte  SanitizeFeatureSupported : 1;
  byte  CryptoScrambleExtCommandSupported : 1;
  byte  OverwriteExtCommandSupported : 1;
  byte  BlockEraseExtCommandSupported : 1;
  dword  UserAddressableSectors;
  word ObsoleteWord62;
  word MultiWordDMASupport : 8;
  word MultiWordDMAActive : 8;
  word AdvancedPIOModes : 8;
  word ReservedByte64 : 8;
  word MinimumMWXferCycleTime;
  word RecommendedMWXferCycleTime;
  word MinimumPIOCycleTime;
  word MinimumPIOCycleTimeIORDY;
  struct {
    word ZonedCapabilities : 2;
    word NonVolatileWriteCache : 1;
    word ExtendedUserAddressableSectorsSupported : 1;
    word DeviceEncryptsAllUserData : 1;
    word ReadZeroAfterTrimSupported : 1;
    word Optional28BitCommandsSupported : 1;
    word IEEE1667 : 1;
    word DownloadMicrocodeDmaSupported : 1;
    word SetMaxSetPasswordUnlockDmaSupported : 1;
    word WriteBufferDmaSupported : 1;
    word ReadBufferDmaSupported : 1;
    word DeviceConfigIdentifySetDmaSupported : 1;
    word LPSAERCSupported : 1;
    word DeterministicReadAfterTrimSupported : 1;
    word CFastSpecSupported : 1;
  } AdditionalSupported;
  word ReservedWords70[5];
  word QueueDepth : 5;
  word ReservedWord75 : 11;
  struct {
    word Reserved0 : 1;
    word SataGen1 : 1;
    word SataGen2 : 1;
    word SataGen3 : 1;
    word Reserved1 : 4;
    word NCQ : 1;
    word HIPM : 1;
    word PhyEvents : 1;
    word NcqUnload : 1;
    word NcqPriority : 1;
    word HostAutoPS : 1;
    word DeviceAutoPS : 1;
    word ReadLogDMA : 1;
    word Reserved2 : 1;
    word CurrentSpeed : 3;
    word NcqStreaming : 1;
    word NcqQueueMgmt : 1;
    word NcqReceiveSend : 1;
    word DEVSLPtoReducedPwrState : 1;
    word Reserved3 : 8;
  } SerialAtaCapabilities;
  struct {
    word Reserved0 : 1;
    word NonZeroOffsets : 1;
    word DmaSetupAutoActivate : 1;
    word DIPM : 1;
    word InOrderData : 1;
    word HardwareFeatureControl : 1;
    word SoftwareSettingsPreservation : 1;
    word NCQAutosense : 1;
    word DEVSLP : 1;
    word HybridInformation : 1;
    word Reserved1 : 6;
  } SerialAtaFeaturesSupported;
  struct {
    word Reserved0 : 1;
    word NonZeroOffsets : 1;
    word DmaSetupAutoActivate : 1;
    word DIPM : 1;
    word InOrderData : 1;
    word HardwareFeatureControl : 1;
    word SoftwareSettingsPreservation : 1;
    word DeviceAutoPS : 1;
    word DEVSLP : 1;
    word HybridInformation : 1;
    word Reserved1 : 6;
  } SerialAtaFeaturesEnabled;
  word MajorRevision;
  word MinorRevision;
  struct {
    word SmartCommands : 1;
    word SecurityMode : 1;
    word RemovableMediaFeature : 1;
    word PowerManagement : 1;
    word Reserved1 : 1;
    word WriteCache : 1;
    word LookAhead : 1;
    word ReleaseInterrupt : 1;
    word ServiceInterrupt : 1;
    word DeviceReset : 1;
    word HostProtectedArea : 1;
    word Obsolete1 : 1;
    word WriteBuffer : 1;
    word ReadBuffer : 1;
    word Nop : 1;
    word Obsolete2 : 1;
    word DownloadMicrocode : 1;
    word DmaQueued : 1;
    word Cfa : 1;
    word AdvancedPm : 1;
    word Msn : 1;
    word PowerUpInStandby : 1;
    word ManualPowerUp : 1;
    word Reserved2 : 1;
    word SetMax : 1;
    word Acoustics : 1;
    word BigLba : 1;
    word DeviceConfigOverlay : 1;
    word FlushCache : 1;
    word FlushCacheExt : 1;
    word WordValid83 : 2;
    word SmartErrorLog : 1;
    word SmartSelfTest : 1;
    word MediaSerialNumber : 1;
    word MediaCardPassThrough : 1;
    word StreamingFeature : 1;
    word GpLogging : 1;
    word WriteFua : 1;
    word WriteQueuedFua : 1;
    word WWN64Bit : 1;
    word URGReadStream : 1;
    word URGWriteStream : 1;
    word ReservedForTechReport : 2;
    word IdleWithUnloadFeature : 1;
    word WordValid : 2;
  } CommandSetSupport;
  struct {
    word SmartCommands : 1;
    word SecurityMode : 1;
    word RemovableMediaFeature : 1;
    word PowerManagement : 1;
    word Reserved1 : 1;
    word WriteCache : 1;
    word LookAhead : 1;
    word ReleaseInterrupt : 1;
    word ServiceInterrupt : 1;
    word DeviceReset : 1;
    word HostProtectedArea : 1;
    word Obsolete1 : 1;
    word WriteBuffer : 1;
    word ReadBuffer : 1;
    word Nop : 1;
    word Obsolete2 : 1;
    word DownloadMicrocode : 1;
    word DmaQueued : 1;
    word Cfa : 1;
    word AdvancedPm : 1;
    word Msn : 1;
    word PowerUpInStandby : 1;
    word ManualPowerUp : 1;
    word Reserved2 : 1;
    word SetMax : 1;
    word Acoustics : 1;
    word BigLba : 1;
    word DeviceConfigOverlay : 1;
    word FlushCache : 1;
    word FlushCacheExt : 1;
    word Resrved3 : 1;
    word Words119_120Valid : 1;
    word SmartErrorLog : 1;
    word SmartSelfTest : 1;
    word MediaSerialNumber : 1;
    word MediaCardPassThrough : 1;
    word StreamingFeature : 1;
    word GpLogging : 1;
    word WriteFua : 1;
    word WriteQueuedFua : 1;
    word WWN64Bit : 1;
    word URGReadStream : 1;
    word URGWriteStream : 1;
    word ReservedForTechReport : 2;
    word IdleWithUnloadFeature : 1;
    word Reserved4 : 2;
  } CommandSetActive;
  word UltraDMASupport : 8;
  word UltraDMAActive : 8;
  struct {
    word TimeRequired : 15;
    word ExtendedTimeReported : 1;
  } NormalSecurityEraseUnit;
  struct {
    word TimeRequired : 15;
    word ExtendedTimeReported : 1;
  } EnhancedSecurityEraseUnit;
  word CurrentAPMLevel : 8;
  word ReservedWord91 : 8;
  word MasterPasswordID;
  word HardwareResetResult;
  word CurrentAcousticValue : 8;
  word RecommendedAcousticValue : 8;
  word StreamMinRequestSize;
  word StreamingTransferTimeDMA;
  word StreamingAccessLatencyDMAPIO;
  dword  StreamingPerfGranularity;
  dword  Max48BitLBA[2];
  word StreamingTransferTime;
  word DsmCap;
  struct {
    word LogicalSectorsPerPhysicalSector : 4;
    word Reserved0 : 8;
    word LogicalSectorLongerThan256Words : 1;
    word MultipleLogicalSectorsPerPhysicalSector : 1;
    word Reserved1 : 2;
  } PhysicalLogicalSectorSize;
  word InterSeekDelay;
  word WorldWideName[4];
  word ReservedForWorldWideName128[4];
  word ReservedForTlcTechnicalReport;
  word WordsPerLogicalSector[2];
  struct {
    word ReservedForDrqTechnicalReport : 1;
    word WriteReadVerify : 1;
    word WriteUncorrectableExt : 1;
    word ReadWriteLogDmaExt : 1;
    word DownloadMicrocodeMode3 : 1;
    word FreefallControl : 1;
    word SenseDataReporting : 1;
    word ExtendedPowerConditions : 1;
    word Reserved0 : 6;
    word WordValid : 2;
  } CommandSetSupportExt;
  struct {
    word ReservedForDrqTechnicalReport : 1;
    word WriteReadVerify : 1;
    word WriteUncorrectableExt : 1;
    word ReadWriteLogDmaExt : 1;
    word DownloadMicrocodeMode3 : 1;
    word FreefallControl : 1;
    word SenseDataReporting : 1;
    word ExtendedPowerConditions : 1;
    word Reserved0 : 6;
    word Reserved1 : 2;
  } CommandSetActiveExt;
  word ReservedForExpandedSupportandActive[6];
  word MsnSupport : 2;
  word ReservedWord127 : 14;
  struct {
    word SecuritySupported : 1;
    word SecurityEnabled : 1;
    word SecurityLocked : 1;
    word SecurityFrozen : 1;
    word SecurityCountExpired : 1;
    word EnhancedSecurityEraseSupported : 1;
    word Reserved0 : 2;
    word SecurityLevel : 1;
    word Reserved1 : 7;
  } SecurityStatus;
  word ReservedWord129[31];
  struct {
    word MaximumCurrentInMA : 12;
    word CfaPowerMode1Disabled : 1;
    word CfaPowerMode1Required : 1;
    word Reserved0 : 1;
    word Word160Supported : 1;
  } CfaPowerMode1;
  word ReservedForCfaWord161[7];
  word NominalFormFactor : 4;
  word ReservedWord168 : 12;
  struct {
    word SupportsTrim : 1;
    word Reserved0 : 15;
  } DataSetManagementFeature;
  word AdditionalProductID[4];
  word ReservedForCfaWord174[2];
  word CurrentMediaSerialNumber[30];
  struct {
    word Supported : 1;
    word Reserved0 : 1;
    word WriteSameSuported : 1;
    word ErrorRecoveryControlSupported : 1;
    word FeatureControlSuported : 1;
    word DataTablesSuported : 1;
    word Reserved1 : 6;
    word VendorSpecific : 4;
  } SCTCommandTransport;
  word ReservedWord207[2];
  struct {
    word AlignmentOfLogicalWithinPhysical : 14;
    word Word209Supported : 1;
    word Reserved0 : 1;
  } BlockAlignment;
  word WriteReadVerifySectorCountMode3Only[2];
  word WriteReadVerifySectorCountMode2Only[2];
  struct {
    word NVCachePowerModeEnabled : 1;
    word Reserved0 : 3;
    word NVCacheFeatureSetEnabled : 1;
    word Reserved1 : 3;
    word NVCachePowerModeVersion : 4;
    word NVCacheFeatureSetVersion : 4;
  } NVCacheCapabilities;
  word NVCacheSizeLSW;
  word NVCacheSizeMSW;
  word NominalMediaRotationRate;
  word ReservedWord218;
  struct {
    byte NVCacheEstimatedTimeToSpinUpInSeconds;
    byte Reserved;
  } NVCacheOptions;
  word WriteReadVerifySectorCountMode : 8;
  word ReservedWord220 : 8;
  word ReservedWord221;
  struct {
    word MajorVersion : 12;
    word TransportType : 4;
  } TransportMajorVersion;
  word TransportMinorVersion;
  word ReservedWord224[6];
  dword  ExtendedNumberOfUserAddressableSectors[2];
  word MinBlocksPerDownloadMicrocodeMode03;
  word MaxBlocksPerDownloadMicrocodeMode03;
  word ReservedWord236[19];
  word Signature : 8;
  word CheckSum : 8;
} IDENTIFY_DEVICE_DATA;

#endif