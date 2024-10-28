#include "NightmaresClient.h"

void UNightmaresClient::Connect(const FString& Ip, const int Port)
{
	OnChangeConnectState.Broadcast(Connecting);
	ClientPrivate = MakeShared<alice::network::Connection>(IOPrivate, TCHAR_TO_ANSI(*Ip), Port);
	ClientPrivate->on_disconnect_callbacks.push_back([this]()
	{
		OnChangeConnectState.Broadcast(Disconnected);
	});
	ClientPrivate->on_connect_callbacks.push_back([this]()
	{
		OnChangeConnectState.Broadcast(Connected);
	});
	ClientPrivate->on_read_callbacks.emplace_back([this](const std::string& Message)
	{
		OnRead.Broadcast(ANSI_TO_TCHAR(Message.c_str()));
	});
}

void UNightmaresClient::Update()
{
	IOPrivate.poll();
}

void UNightmaresClient::Send(const FString& Msg)
{
	if(ClientPrivate)
	{
		ClientPrivate->Send(TCHAR_TO_ANSI(*Msg));
	}
}
