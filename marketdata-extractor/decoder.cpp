#include "headers/decoder.h"
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <iostream>

SimbaDecoder::SimbaDecoder(const std::vector<uint8_t>& packet_data) : packetData_(packet_data), offset(0)
, marketpacketHeader{}, incrementalpacketHeader{}, snapshot{} {
}

std::string SimbaDecoder::formatTimestamp(uint64_t ns_timestamp) {

	std::chrono::nanoseconds ns(ns_timestamp);
	auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>(ns);
	auto tp_cast = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp);
	std::time_t tt = std::chrono::system_clock::to_time_t(tp_cast);
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(ns);
	auto ns_remainder = ns - seconds;
	std::tm local_tm;
	localtime_s(&local_tm, &tt);

	std::ostringstream oss;
	oss << std::put_time(&local_tm, "%Y%m%d%H%M%S") << std::setfill('0') << std::setw(9) << ns_remainder.count();

	return oss.str();
}


std::string SimbaDecoder::decodeMsgFlagsTostdstring(uint16_t msgFlags) {
	std::ostringstream oss;
	oss << "{\"LastFragment\":" << ((msgFlags & (1 << 0)) ? 1 : 0) << ","
		<< "\"StartOfSnapshot\":" << ((msgFlags & (1 << 1)) ? 1 : 0) << ","
		<< "\"EndOfSnapshot\":" << ((msgFlags & (1 << 2)) ? 1 : 0) << ","
		<< "\"IncrementalPacket\":" << ((msgFlags & (1 << 3)) ? 1 : 0) << ","
		<< "\"PossDupFlag\":" << ((msgFlags & (1 << 4)) ? 1 : 0)
		<< "}";
	return oss.str();
}

MarketDataPacketHeader SimbaDecoder::decodeMarketDataPacketHeader() {
	if (packetData_.size() < sizeof(MarketDataPacketHeader)) {
		throw std::runtime_error("Small packet Data.");
	}
	MarketDataPacketHeader marketpacketHeader;
	memcpy(&marketpacketHeader, &packetData_[offset], sizeof(MarketDataPacketHeader));
	offset += sizeof(MarketDataPacketHeader);
	return marketpacketHeader;
}

IncrementalPacketHeader SimbaDecoder::decodeIncrementalPacketHeader() {
	if (offset + sizeof(IncrementalPacketHeader) > packetData_.size()) {
		throw std::runtime_error("Error: Incomplete IncrementalPacketHeader.");
	}

	IncrementalPacketHeader incrementalpacketHeader;
	memcpy(&incrementalpacketHeader, &packetData_[offset], sizeof(IncrementalPacketHeader));
	offset += sizeof(IncrementalPacketHeader);
	return incrementalpacketHeader;
}



messageHeader SimbaDecoder::decodeMessageHeader() {
	if (offset + sizeof(messageHeader) > packetData_.size()) {
		throw std::runtime_error("Error: Incomplete Header message.");
	}

	messageHeader header;
	memcpy(&header, &packetData_[offset], sizeof(messageHeader));
	offset += sizeof(messageHeader);
	return header;
}

OrderUpdate SimbaDecoder::decodeOrderUpdate() {

	if (offset + sizeof(OrderUpdate) > packetData_.size()) {
		throw std::runtime_error("Error: Incomplete OrderUpdate message");
	}

	OrderUpdate order;
	memcpy(&order, &packetData_[offset], sizeof(OrderUpdate));
	offset += sizeof(OrderUpdate);
	return order;
}

OrderExecution SimbaDecoder::decodeOrderExecution() {

	if (offset + sizeof(OrderExecution) > packetData_.size()) {
		throw std::runtime_error("Error: Incomplete OrderExecution message");
	}

	OrderExecution exec;
	memcpy(&exec, &packetData_[offset], sizeof(OrderExecution));
	offset += sizeof(OrderExecution);
	return exec;
}

OrderBookSnapshot SimbaDecoder::decodeOrderBookSnapshot() {
	if (offset + sizeof(OrderBookSnapshot) > packetData_.size()) {
		throw std::runtime_error("Error: Incomplete OrderBook Snapshot message");
	}

	OrderBookSnapshot snapshot;
	memcpy(&snapshot, &packetData_[offset], sizeof(OrderBookSnapshot));
	offset += sizeof(OrderBookSnapshot);

	repeatingSection.resize(snapshot.NoMDEntries.numInGroup);
	for (auto& rs : repeatingSection) {
		memcpy(&rs, &packetData_[offset], sizeof(OrderBookSnapshot::RepeatingSection));
		offset += sizeof(OrderBookSnapshot::RepeatingSection);
	}
	return snapshot;
}


void SimbaDecoder::decode() {
	marketpacketHeader = decodeMarketDataPacketHeader();

	if (marketpacketHeader.IsIncremental()) {
		incrementalpacketHeader = decodeIncrementalPacketHeader();
	}

	while (offset < packetData_.size()) {
		header.emplace_back(decodeMessageHeader());

		switch (header.back().templateID)
		{
		case OrderUpdate::Template_ID:
			orderUpdates.emplace_back(decodeOrderUpdate());
			break;
		case OrderExecution::Template_ID:
			orderExecutions.emplace_back(decodeOrderExecution());
			break;
		case OrderBookSnapshot::Template_ID:
			decodeOrderBookSnapshot();
			break;
		default:
			offset = packetData_.size();
			break;
		}
	}

}

void SimbaDecoder::AddHeaderToJSON(const messageHeader& header) {
	json << "{\"SBEHeader\": {\"BlockLength\":" << header.blockLength << ",";
	json << "\"TemplateID\":" << header.templateID << ",";
	json << "\"SchemaID\":" << header.schemaID << ",";
	json << "\"Version\":" << header.version << "}";
}

void SimbaDecoder::toJSON() {

	json << "{\"PacketHeader\":{";
	json << "\"MsgSeqNum\":" << marketpacketHeader.msgSeqNum << ",";
	json << "\"MsgSize\":" << marketpacketHeader.msgSize << ",";
	json << "\"MsgFlags\":" << decodeMsgFlagsTostdstring(marketpacketHeader.msgFlags) << ",";
	json << "\"SendingTime\":" << formatTimestamp(marketpacketHeader.sendingTime) << "}";

	if (marketpacketHeader.IsIncremental()) {
		json << ",\"IncrementalPacketHeader\": {\"TransactTime\":" << formatTimestamp(incrementalpacketHeader.transactTime) << ",";
		json << "\"ExchangeTradingSessionID\":" << incrementalpacketHeader.exchangeTradingSessionID << "},\"Messages\": [";
	}
	else {
		json << ",\"Messages\": [";
	}
	auto orderUpdatesItr = orderUpdates.begin();
	auto orderExecutionsItr = orderExecutions.begin();

	for (const auto& header_element : header) {

		AddHeaderToJSON(header_element);
		switch (header_element.templateID)
		{
		case OrderUpdate::Template_ID:
			if (orderUpdatesItr != orderUpdates.end()) {

				const auto& order = *orderUpdatesItr;

				json << ",\"SBEMessage\":{\"MDUpdateAction\":" << static_cast<int>(order.MDUpdateAction) << ",";
				json << "\"MDEntryType\":" << static_cast<char>(order.MDEntryType) << ",";
				json << "\"MDEntryID\":" << order.MDEntryID << ",";
				json << "\"SecurityID\":" << order.SecurityID << ",";
				json << "\"RptSeq\":" << order.RptSeq << ",";
				json << "\"MDEntryPx\":" << order.MDEntryPx.mantissa * order.MDEntryPx.exponent << ",";
				json << "\"MDEntrySize\":" << order.MDEntrySize << ",";
				json << "\"MDFlags\":" << static_cast<uint64_t>(order.MDFlags) << ",";
				json << "\"MDFlags2\":" << order.MDFlags2;
				++orderUpdatesItr;
			}
			break;

		case OrderExecution::Template_ID:
			if (orderExecutionsItr != orderExecutions.end()) {

				const auto& exec = *orderExecutionsItr;

				json << ",\"SBEMessage\":{\"MDUpdateAction\":" << static_cast<int>(exec.MDUpdateAction) << ",";
				json << "\"MDEntryType\":" << static_cast<char>(exec.MDEntryType) << ",";
				json << "\"MDEntryID\":" << exec.MDEntryID << ",";
				json << "\"SecurityID\":" << exec.SecurityID << ",";
				json << "\"RptSeq\":" << exec.RptSeq << ",";
				json << "\"MDEntryPx\":" << (exec.MDEntryPx.mantissa != Decimal5NULL::NULL_VALUE
					? exec.MDEntryPx.mantissa * exec.MDEntryPx.exponent : 0) << ",";
				json << "\"LastPx\":" << exec.LastPx.mantissa * exec.LastPx.exponent << ",";
				json << "\"LastQty\":" << exec.LastQty << ",";
				json << "\"TradeID\":" << exec.TradeID << ",";
				json << "\"MDFlags\":" << static_cast<uint64_t>(exec.MDFlags);
				++orderExecutionsItr;
			}
			break;
		case OrderBookSnapshot::Template_ID:
			json << ",\"OrderBookSnapshot\":{\"SecurityID\":" << snapshot.SecurityID << ",";
			json << "\"LastMsgSeqNumProcessed\":" << snapshot.LastMsgSeqNumProcessed << ",";
			json << "\"RptSeq\":" << snapshot.RptSeq << ",";
			json << "\"ExchangeTradingSessionID\":" << snapshot.ExchangeTradingSessionID << ",";
			json << "\"NoMDEntries\":{\"block_length\":" << snapshot.NoMDEntries.blockLength << ",\"num_in_group\":"
				<< static_cast<int>(snapshot.NoMDEntries.numInGroup) << "},\"RepeatingSections\":[";
			for (auto& rp : repeatingSection) {

				json << "{\"MDEntryID\":" << rp.MDEntryID << ",";
				json << "\"TransactTime\":" << rp.TransactTime << ",";
				json << "\"MDEntryPx\":" << (rp.MDEntryPx.mantissa != Decimal5NULL::NULL_VALUE
					? rp.MDEntryPx.mantissa * rp.MDEntryPx.exponent
					: 0) << ",";
				json << "\"MDEntrySize\":" << rp.MDEntrySize << ",";
				json << "\"TradeID\":" << rp.TradeID << ",";
				json << "\"MDFlags\":" << static_cast<uint64_t>(rp.MDFlags) << ",";
				json << "\"MDFlags2\":" << rp.MDFlags2 << ",";
				json << "\"MDEntryType\":" << static_cast<uint64_t>(rp.MDEntryType);

				if (&rp == &repeatingSection.back()) {
					json << "}]";
				}
				else {
					json << "},";
				}

			}

			break;
		default:
			json << ",\"SBEMessage\":{";
			break;
		}
		if (&header_element == &header.back()) {
			json << "}}]";
		}
		else {
			json << "}},";
		}
	}
	json << "}";

}

std::ostringstream& SimbaDecoder::getJSON() {
	return json;
}