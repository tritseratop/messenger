#include "ClientListener.hpp"
#include "../Chat.hpp"

v_int64 ClientListener::getClientId() {
	return clientId;
}

oatpp::async::CoroutineStarter ClientListener::onPing(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message) {
	OATPP_LOGI(TAG, "onPing");
	return socket->sendPongAsync(message);
}
oatpp::async::CoroutineStarter ClientListener::onPong(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message) {
	OATPP_LOGI(TAG, "onPong");
	return nullptr;
}
oatpp::async::CoroutineStarter ClientListener::onClose(const std::shared_ptr<AsyncWebSocket>& socket, v_uint16 code, const oatpp::String& message) {
	OATPP_LOGD(TAG, "onClose code = %d", code);
	return nullptr;
}
oatpp::async::CoroutineStarter ClientListener::readMessage(const std::shared_ptr<AsyncWebSocket>& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) {
	if (size == 0) {
		auto wholeMessage = m_messageBuffer.toString();
		m_messageBuffer.clear();
		OATPP_LOGD(TAG, "onMessage message ='%s'", wholeMessage->c_str());
		m_chat->sendMessageToAllAsync(wholeMessage); // SENDING
		return nullptr;
	}
	else if (size > 0) {
		m_messageBuffer.writeSimple(data, size);
	}
	return nullptr;
}

void ClientListener::sendMessageAsync(const oatpp::String& message) {
	class SendMessageCoroutine : public oatpp::async::Coroutine<SendMessageCoroutine> {
	private:
		std::shared_ptr<AsyncWebSocket> websocket;
		oatpp::String message;
	public:
		SendMessageCoroutine(const std::shared_ptr<AsyncWebSocket> socket, const oatpp::String& message = "Hello")
			: websocket(socket)
			, message(message)
		{}

		Action act() override {
			return websocket->sendOneFrameTextAsync(message).next(finish());
		}
	};

	if (m_socket) {
		m_asyncExecutor->execute<SendMessageCoroutine>(m_socket, message);
	}
}

void ClientListener::invalidateSocket() {
	if (m_socket) {
		m_serverConnectionProvider->invalidate(m_socket->getConnection());
	}
	m_socket.reset();
}