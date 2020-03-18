#pragma once
#include "Poco/Channel.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"
#include "Poco/NotificationQueue.h"
#include "Poco/Net/WebSocket.h"

namespace Reach {
	using namespace Poco;
	using Poco::Net::WebSocket;
	class WebSocketChannel : public Channel, public Runnable
	{
	public:
		WebSocketChannel(Channel* pChannel = 0, Thread::Priority proi = Thread::PRIO_NORMAL);
		void setChannel(Channel* pChannel);
		Channel* getChannel() const;
		void open();
		void close();
		void ready(HWND hwnd);
		void log(const Message& msg);
		void setPropery(const std::string& name, const std::string& value);
	protected:
		~WebSocketChannel();
		void init();
		void run();
		void setPriority(const std::string& value);
		void registerService();
		void unregisterService();
	private:
		WebSocket*	_pSocket;
		Channel*	_pChannel;
		Thread		_thread;
		HWND		_hWnd;
		FastMutex	_threadMutex;
		FastMutex	_channelMutex;
		NotificationQueue	_queue;
		bool _needStop;
	};
}