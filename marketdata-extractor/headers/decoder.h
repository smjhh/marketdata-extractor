#pragma once
#include <vector>
#include <iostream>
#include <sstream>



#pragma pack(push, 1)

struct Decimal5
{
	int64_t mantissa;
	static constexpr double exponent = 1e-5;
};

struct Decimal5NULL
{
	int64_t mantissa;
	static constexpr double exponent = 1e-5;
	static constexpr int64_t MAX_VALUE = 9223372036854775806;
	static constexpr int64_t NULL_VALUE = 9223372036854775807;
};

// GroupSize2
struct groupSize2 {
	uint16_t blockLength;
	uint16_t numInGroup;
};

// GroupSize
struct groupSize {
	uint16_t blockLength;
	uint8_t numInGroup;
};

// Market Data Packet Header
struct MarketDataPacketHeader {
	uint32_t msgSeqNum;
	uint16_t msgSize;
	uint16_t msgFlags;
	uint64_t sendingTime;
	bool IsIncremental() const { return msgFlags & 0x8; }
};

//Incremental Packet Header
struct IncrementalPacketHeader {
	uint64_t transactTime;
	uint32_t exchangeTradingSessionID;
	static constexpr uint32_t exchangeTradingSessionID_nullValue = 4294967295;
};

// SBE Message Header
struct messageHeader {
	uint16_t blockLength;
	uint16_t templateID;
	uint16_t schemaID;
	uint16_t version;
};

// Enumeration for MDFlagsSet
enum class MDFlagsSet : uint64_t
{
	Day = 0x1,
	IOC = 0x2,
	NonQuote = 0x4,
	EndOfTransaction = 0x1000,
	SecondLeg = 0x4000,
	FOK = 0x80000,
	Replace = 0x100000,
	Cancel = 0x200000,
	MassCancel = 0x400000,
	Negotiated = 0x4000000,
	MultiLeg = 0x8000000,
	CrossTrade = 0x20000000,
	COD = 0x100000000,
	ActiveSide = 0x20000000000,
	PassiveSide = 0x40000000000,
	Synthetic = 0x200000000000,
	RFS = 0x400000000000,
	SyntheticPassive = 0x200000000000000,
};

// Enumeration for MDUpdateAction
enum class MDUpdateAction : uint8_t
{
	New = 0,
	Change = 1,
	Delete = 2,
};

// Enumeration for MDEntryType
enum class MDEntryType : char
{
	Bid = '0',
	Offer = '1',
	EmptyBook = 'J',
};

// Order Update Message (msg id=15)
struct OrderUpdate {
	int64_t MDEntryID;
	Decimal5 MDEntryPx;
	int64_t MDEntrySize;
	MDFlagsSet MDFlags;
	uint64_t MDFlags2;
	int32_t SecurityID;
	uint32_t RptSeq;
	MDUpdateAction MDUpdateAction;
	MDEntryType MDEntryType;
	static constexpr uint16_t Template_ID = 15;

};

// Order Execution Message (msg id=16)
struct OrderExecution {
	int64_t MDEntryID;
	Decimal5NULL MDEntryPx;
	int64_t MDEntrySize;
	Decimal5 LastPx;
	int64_t LastQty;
	int64_t TradeID;
	MDFlagsSet MDFlags;
	uint64_t MDFlags2;
	int32_t SecurityID;
	uint32_t RptSeq;
	MDUpdateAction MDUpdateAction;
	MDEntryType MDEntryType;
	static constexpr uint16_t Template_ID = 16;
};

// Order Book Snapshot Message (msg id=17)
struct OrderBookSnapshot {
	int32_t SecurityID;
	uint32_t LastMsgSeqNumProcessed;
	uint32_t RptSeq;
	uint32_t ExchangeTradingSessionID;
	groupSize NoMDEntries;

	struct RepeatingSection {
		int64_t MDEntryID;
		uint64_t TransactTime;
		Decimal5NULL MDEntryPx;
		int64_t MDEntrySize;
		int64_t TradeID;
		MDFlagsSet MDFlags;
		uint64_t MDFlags2;
		MDEntryType MDEntryType;
	};
	static constexpr uint16_t Template_ID = 17;
};

#pragma pack(pop)

class Decoder {
public:
	explicit Decoder(const std::vector<uint8_t>& packet_data);
	void decode();
	void toJSON();
	std::ostringstream& getJSON();
private:
	size_t offset;
	MarketDataPacketHeader marketpacketHeader;
	IncrementalPacketHeader incrementalpacketHeader;

	const std::vector<uint8_t>& packetData_;
	std::string decodeMsgFlagsTostdstring(uint16_t msgFlags);
	std::string formatTimestamp(uint64_t milliseconds);
	std::vector<messageHeader> header;
	std::vector<OrderUpdate> orderUpdates;
	std::vector<OrderExecution> orderExecutions;
	OrderBookSnapshot snapshot;
	std::ostringstream json;


	MarketDataPacketHeader decodeMarketDataPacketHeader();
	IncrementalPacketHeader decodeIncrementalPacketHeader();
	messageHeader decodeMessageHeader();
	OrderUpdate decodeOrderUpdate();
	OrderExecution decodeOrderExecution();
	OrderBookSnapshot decodeOrderBookSnapshot();
	std::vector<OrderBookSnapshot::RepeatingSection> repeatingSection;

	void AddHeaderToJSON(const messageHeader& header);
	

};
