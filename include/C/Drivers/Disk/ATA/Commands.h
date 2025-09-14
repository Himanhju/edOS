#ifndef ATA_COMMANDS
#define ATA_COMMANDS

typedef enum ATA_PRESENT_COMMANDS{
    NOP =                       0x00,

    REQ_EX_ERR_CODE =           0x03,

    DATA_SET_MNGMT =            0x06,
    DATA_SET_MNGMT_XL =         0x07,

    REQ_SENSE_DATA_EXT=         0x0B,

    GET_PHY_EMENT_STAT=         0x12,

    READ_SECTORS =              0x20,

    READ_SECTORS_EXT =          0x24,
    READ_DMA_EXT =              0x25,

    READ_STREAM_DMA_EXT=        0x2A,
    READ_STREAM_EXT =           0x2B,

    READ_LOG_EXT =              0x2F,
    WRITE_SECTORS =             0x30,

    WRITE_SECTORS_EXT =         0x34,
    WRITE_DMA_EXT =             0x35,

    CFA_WRITE_SECTOR_WO_ERASE = 0x38,

    WRITE_STREAM_DMA_EXT =      0x3A,
    WRITE_STRAM_EXT =           0x3B,

    IDENTIFY_DEVICE =           0xEC
}ATA_PRESENT_COMMANDS;
/*
00	NOP	none	8-bit	ATA-1 to present			
03	CFA REQUEST EXTENDED ERROR CODE	none	8-bit	ATA-4 to present		
06	DATA SET MANAGEMENT	DMA	16-bit	ACS-2 to present
07	DATA SET MANAGEMENT XL	DMA	16-bit	ACS-4 to present			
0B	REQUEST SENSE DATA EXT	none	16-bit	ACS-2 to present
12	GET PHYSICAL ELEMENT STATUS	DMA	16-bit	ACS-4 to present
20	READ SECTOR(S)	PIO	8-bit	IBM PC/AT to present
24	READ SECTOR(S) EXT	PIO	16-bit	ATA-6 to present
25	READ DMA EXT	DMA	16-bit	ATA-6 to present
2A	READ STREAM DMA EXT	DMA	16-bit	ATA-7 to present
2B	READ STREAM EXT	PIO	16-bit	ATA-7 to present
2F	READ LOG EXT	PIO	16-bit	ATA-6 to present
30	WRITE SECTOR(S)	PIO	8-bit	IBM PC/AT to present
34	WRITE SECTOR(S) EXT	PIO	16-bit	ATA-6 to present
35	WRITE DMA EXT	DMA	16-bit	ATA-6 to present
38	CFA WRITE SECTORS WITHOUT ERASE	PIO	8-bit	ATA-4 to present
3A	WRITE STREAM DMA EXT	DMA	16-bit	ATA-7 to present
3B	WRITE STREAM EXT	PIO	16-bit	ATA-7 to present

3D	WRITE DMA FUA EXT	DMA	16-bit	ATA-7 to present
3F	WRITE LOG EXT	PIO	16-bit	ATA-6 to present
40	READ VERIFY SECTOR(S)	none	8-bit	IBM PC/AT to present
42	READ VERIFY SECTOR(S) EXT	none	16-bit	ATA-6 to present
44	ZERO EXT	none	16-bit	ACS-4 to present
45	WRITE UNCORRECTABLE EXT	none	16-bit	ATA8-ACS to present
47	READ LOG DMA EXT	DMA	16-bit	ATA8-ACS to present
4A	ZAC Management In	DMA	16-bit	ACS-4 to present
51	CONFIGURE STREAM	none	16-bit	ATA-7 to present
57	WRITE LOG DMA EXT	DMA	16-bit	ATA8-ACS to present
5B	TRUSTED NON-DATA	none	8-bit	ACS-2 to present
5C	TRUSTED RECEIVE	PIO	8-bit	ATA8-ACS to present
5D	TRUSTED RECEIVE DMA	DMA	8-bit	ATA8-ACS to present
5E	TRUSTED SEND	PIO	8-bit	ATA8-ACS to present
5F	TRUSTED SEND DMA	DMA	8-bit	ATA8-ACS to present
60	READ FPDMA QUEUED	queued DMA	16-bit	ATA8-ACS to present
61	WRITE FPDMA QUEUED	queued DMA	16-bit	ATA8-ACS to present
63	NCQ NON-DATA	none	16-bit	ACS-3 to present
64	SEND FPDMA QUEUED	queued DMA	16-bit	ACS-3 to present
65	RECEIVE FPDMA QUEUED	queued DMA	16-bit	ACS-3 to present
77	SET DATE & TIME EXT	none	16-bit	ACS-3 to present
78	ACCESSIBLE MAX ADDRESS CONFIGURATION	none	16-bit	ACS-3 to present
7C	REMOVE ELEMENT AND TRUNCATE	none	16-bit	ACS-4 to present
7D	RESTORE ELEMENTS AND REBUILD	none	16-bit	ACS-5 to present
7E	REMOVE ELEMENT AND MODIFY ZONES	none	16-bit	ACS-5 to present	
87	CFA TRANSLATE SECTOR	PIO	8-bit	ATA-4 to present
90	EXECUTE DEVICE DIAGNOSTIC	none	8-bit	IBM PC/AT to present
92	DOWNLOAD MICROCODE	PIO	8-bit	ATA-2 to present
93	DOWNLOAD MICROCODE DMA	DMA	8-bit	ACS-2 to present
96	MUTATE EXT	none	16-bit	ACS-5 to present
9F	ZAC Management Out	DMA	16-bit	ACS-4 to present			
B0	SMART	PIO	8-bit	ATA-3 to present
B2	SET SECTOR CONFIGURATON EXT	none	16-bit	ACS-4 to present
B4	Sanitize Device	none	16-bit	ACS-2 to present
C8	READ DMA	DMA	8-bit	ATA-1 to present
CA	WRITE DMA	DMA	8-bit	ATA-1 to present
CD	CFA WRITE MULTIPLE WITHOUT ERASE	PIO	8-bit	ATA-4 to present
E0	STANDBY IMMEDIATE	none	8-bit	ATA-1 to present
E1	IDLE IMMEDIATE	none	8-bit	ATA-1 to present
E2	STANDBY	none	8-bit	ATA-1 to present
E3	IDLE	none	8-bit	ATA-1 to present
E4	READ BUFFER	PIO	8-bit	ATA-1 to present
E5	CHECK POWER MODE	none	8-bit	ATA-1 to present
E6	SLEEP	none	8-bit	ATA-1 to present
E7	FLUSH CACHE	none	8-bit	ATA-4 to present
E8	WRITE BUFFER	PIO	8-bit	ATA-1 to present
E9	READ BUFFER DMA	DMA	8-bit	ACS-2 to present
EA	FLUSH CACHE EXT	none	8-bit	ATA-6 to present
EB	WRITE BUFFER DMA	DMA	8-bit	ACS-2 to present
EC	IDENTIFY DEVICE	PIO	8-bit	ATA-1 to present
EF	SET FEATURES	none	8-bit	ATA-1 to present
F1	SECURITY SET PASSWORD	PIO	8-bit	ATA-3 to present
F2	SECURITY UNLOCK	PIO	8-bit	ATA-3 to present
F3	SECURITY ERASE PREPARE	none	8-bit	ATA-3 to present
F4	SECURITY ERASE UNIT	PIO	8-bit	ATA-3 to present
F5	SECURITY FREEZE LOCK	none	8-bit	ATA-3 to present
F6	SECURITY DISABLE PASSWORD	PIO	8-bit	ATA-3 to present
*/

#endif