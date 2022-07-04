#include "ledgercpp/error.hpp"
#include "ledgercpp/ledger.hpp"
#include "ledgercpp/utils.hpp"

namespace ledger {
	Ledger::Ledger() : transport_(std::make_unique<Transport>(Transport::TransportType::HID)) {

	}

	Ledger::~Ledger() {
		transport_->close();
	}

	Error Ledger::open() {
		return transport_->open();
	}

	std::tuple<ledger::Error, std::vector<uint8_t>> Ledger::get_public_key(std::vector<uint8_t> account, bool confirm) {
		auto [err, buffer] = transport_->exchange(APDU::CLA_NEAR, APDU::INS_GET_PUBLIC_KEY_NEAR, confirm, 87, account);
		if (err != Error::SUCCESS)
			return {err, {}};
		return {err, std::vector<uint8_t>(buffer.begin() + 1, buffer.end())};
	}

	std::tuple<Error, std::vector<uint8_t>> Ledger::sign(std::vector<uint8_t> account, const std::vector<uint8_t>& msg) {
		auto payload = account;
		payload.insert(payload.end(), msg.begin(), msg.end());
		auto [err, buffer] = transport_->exchange(APDU::CLA_NEAR, APDU::INS_SIGN_NEAR, 0x00, 87, payload);
		if (err != Error::SUCCESS)
			return {err, {}};
		return {err, std::vector<uint8_t>(buffer.begin() + 1, buffer.end())};
	}

	void Ledger::close() {
		return transport_->close();
	}
}
