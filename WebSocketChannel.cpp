#include "pch.h"
#include "WebSocketChannel.h"
#include "Poco/AutoPtr.h"
#include "Poco/Message.h"
#include "Poco/LoggingRegistry.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/NetException.h"
#include "Poco/Format.h"

using namespace Reach;
using namespace Poco::Net;

class MQTTMessageNotification : public Notification
{
public:
	MQTTMessageNotification(const Message& msg) :
		_msg(msg)
	{
	}

	~MQTTMessageNotification()
	{
	}

	const Message& message() const
	{
		return _msg;
	}

private:
	Message _msg;
};

WebSocketChannel::WebSocketChannel(Channel* pChannel, Thread::Priority proi) :
	_pChannel(pChannel),
	_thread("WebSocketChannel"),
	_hWnd(0),
	_needStop(false),
	_pSocket(0)
{
	init();
	if (!_pSocket) throw Poco::Exception("Exception!","WebSocketChannel");
	if (_pChannel) _pChannel->duplicate();
	_thread.setPriority(proi);
}

WebSocketChannel::~WebSocketChannel()
{
	try
	{
		unregisterService();
		close();
		if (_pChannel) _pChannel->release();
		if (_pSocket) delete _pSocket;
	}
	catch (...)
	{
		poco_unexpected();
	}
}

void WebSocketChannel::init()
{
	OutputDebugStringA("Begin WebSocketChannel::init\n");

	HTTPClientSession cs("127.0.0.1", 11200);
	HTTPRequest request(HTTPRequest::HTTP_GET, "/ws", HTTPRequest::HTTP_1_1);
	HTTPResponse response;
	try 
	{
		_pSocket = new WebSocket(cs, request, response); 
		poco_assert(_pSocket);
	}
	catch (Poco::Net::NetException&) 
	{
		poco_unexpected();
	}

	OutputDebugStringA("End WebSocketChannel::init\n");
}

void WebSocketChannel::run()
{
	OutputDebugStringA("WebSocketChannel::run() \n");
	const int msgSize = 10240;
	char buffer[msgSize + 1] = { 0 };
	int flags;
	int n = 0;
	do
	{
		OutputDebugStringA(
			Poco::format("thread:%s tid:%lu, WebSocketChannel receiveFrame block\n",
				Thread::current()->name(), Thread::currentTid()).c_str());

		poco_assert(_pSocket);
		try
		{
			n = _pSocket->receiveFrame(buffer, msgSize, flags);
			if (_hWnd && PostMessage(_hWnd, UM_EVENT, (WPARAM)buffer, (LPARAM)n))
				OutputDebugStringA("PostMessage\n");
		}
		catch (Poco::Net::WebSocketException&)
		{
			poco_unexpected()
		}
		catch (Poco::TimeoutException&)
		{
			poco_unexpected();
		}
		catch (Poco::Net::NetException&)
		{
			poco_unexpected();
		}

	} while (n > 0 && n < msgSize && !_needStop);

	OutputDebugStringA("WebSocketChannel::run() exit\n");
}

void WebSocketChannel::setPriority(const std::string& value)
{
	Thread::Priority prio = Thread::PRIO_NORMAL;

	if (value == "lowest")
		prio = Thread::PRIO_LOWEST;
	else if (value == "low")
		prio = Thread::PRIO_LOW;
	else if (value == "normal")
		prio = Thread::PRIO_NORMAL;
	else if (value == "high")
		prio = Thread::PRIO_HIGH;
	else if (value == "highest")
		prio = Thread::PRIO_HIGHEST;
	else
		throw InvalidArgumentException("thread priority", value);

	_thread.setPriority(prio);
}

void WebSocketChannel::setChannel(Channel* pChannel)
{
	FastMutex::ScopedLock lock(_channelMutex);

	if (_pChannel) _pChannel->release();
	_pChannel = pChannel;
	if (_pChannel) _pChannel->duplicate();
}

Channel* WebSocketChannel::getChannel() const
{
	return _pChannel;
}

void WebSocketChannel::open()
{
	FastMutex::ScopedLock lock(_threadMutex);

	if (!_thread.isRunning())
		_thread.start(*this);
}

void WebSocketChannel::close()
{
	OutputDebugStringA("Begin WebSocketChannel close\n");

	if (_thread.isRunning())
	{
		do
		{
			if (_pSocket)
			{
				_pSocket->shutdown();
				_pSocket->close();
			}

			_needStop = true;

		} while (!_thread.tryJoin(100));
	}
	OutputDebugStringA("End WebSocketChannel close\n");
}

void WebSocketChannel::registerService()
{
	poco_assert(_pSocket);
	if (_pSocket)
	{
		std::string payload("registerMqttService");
		_pSocket->sendFrame(payload.data(), (int)payload.size());
	}
}

void WebSocketChannel::unregisterService()
{
	poco_assert(_pSocket);
	if (_pSocket)
	{
		std::string payload("unregisterMqttService");
		_pSocket->sendFrame(payload.data(), (int)payload.size());
	}
}


void WebSocketChannel::ready(HWND hwnd)
{
	OutputDebugStringA("Begin WebSocketChannel ready\n");
	open();

	try
	{
		registerService();

		FastMutex::ScopedLock lock(_channelMutex);
		_hWnd = hwnd;
	}
	catch (NetException&)
	{
		throw;
	}
	OutputDebugStringA("End WebSocketChannel ready\n");
}

void WebSocketChannel::log(const Message& msg)
{
	open();

	_queue.enqueueNotification(new MQTTMessageNotification(msg));
}

void WebSocketChannel::setPropery(const std::string& name, const std::string& value)
{
	if (name == "channel")
		setChannel(LoggingRegistry::defaultRegistry().channelForName(value));
	else if (name == "priority")
		setPriority(value);
	else
		Channel::setProperty(name, value);
}