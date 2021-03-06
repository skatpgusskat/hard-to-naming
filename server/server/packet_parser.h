#pragma once
#include "packet.pb.h"
#include "packet_header.h"
class PacketParser {

public:
	/**
	* @return handled size
	*/
	static std::size_t ParseAndHandlePacket(char* const buffer, std::size_t length) {
		packet::PacketHeader header;
		std::size_t parsed_length = 0;

		while (parsed_length < length) {
			const auto current_buffer = buffer + parsed_length;
			const auto readable_length = length - parsed_length;
			
			std::size_t header_size;
			auto is_parsing_header_successful = header.ParseFromArray(current_buffer, readable_length, header_size);
			if (!is_parsing_header_successful) {
				break;
			}

			auto is_received_not_enough = readable_length < header.length_ + header_size;
			if (is_received_not_enough) {
				break;
			}

			ParseTypeAndCallPacketHandler(header.type_, current_buffer + header_size, header.length_);

			parsed_length += header.length_ + header_size;
		}

		return parsed_length;
	}
private:
	template <typename T>
	static void PacketHandler(T& packet);

	template <typename T>
	static void CallPacketHandler(const char* buffer, std::size_t length) {
		T packet;
		auto is_parsing_successful = packet.ParseFromArray(buffer, length);
		if (!is_parsing_successful) {
			throw std::exception("packet parse fail");
		}

		PacketHandler<T>(packet);
	}

	static void ParseTypeAndCallPacketHandler(google::protobuf::int32 type, const char* buffer, std::size_t length);
};
