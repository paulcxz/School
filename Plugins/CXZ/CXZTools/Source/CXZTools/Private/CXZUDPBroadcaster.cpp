// Fill out your copyright notice in the Description page of Project Settings.

#include "CXZUDPBroadcaster.h"
//#include "Engine.h"

// Sets default values
ACXZUDPBroadcaster::ACXZUDPBroadcaster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	IsInitSender = false;
	IsInitReceiver = false;
	SenderSocket = NULL;
	ReceiverSocket = NULL;
	ReceiverSocket = NULL;
}

// Called when the game starts or when spawned
void ACXZUDPBroadcaster::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACXZUDPBroadcaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACXZUDPBroadcaster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//~~~~~~~~~~~~~~~
	GEngine->AddOnScreenDebugMessage(-1, 55.f, FColor::Red, TEXT("Begin Destroy"));
	if (SenderSocket) //Clear all sockets!
	{
		SenderSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
		SenderSocket = nullptr;
	}
	IsInitSender = false;

	if (ReceiverSocket)
	{
		ReceiverSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ReceiverSocket);
		ReceiverSocket = nullptr;
	}
	IsInitReceiver = false;
}

bool ACXZUDPBroadcaster::UDPBroadcastByteArray(const TArray<uint8>& ToSend)
{
	if(!IsInitSender)
		return false;

	if(!SenderSocket)
	{
		return false;
	}
	//~~~~~~~~~~~~~~~~
	//������Ϣ
	int32 BytesSent = 0;
	int32 size = ToSend.Num();
	if(size > 1024 * 5)
	{
		return false;
	}

	uint8 sendbuf[4096] ={ 0 };
	for(int i=0;i < size;++i)
	{
		sendbuf[i] = ToSend[i];
	}

	SenderSocket->SendTo(sendbuf,size,BytesSent,*RemoteAddr);//���͸�Զ�˵�ַ

	if(BytesSent <= 0)
	{
		const FString Str = "Socket is valid but the receiver sent 0 bytes!";
		UE_LOG(LogTemp,Error,TEXT("%s"),*Str);
		return false;
	}
	else if(BytesSent != size)
	{
		const FString Str = "Socket is valid but the sent bytes not equal to send buffer!";
		UE_LOG(LogTemp,Error,TEXT("%s"),*Str);
		return false;
	}

	//ScreenMsg("UDP Send Succcess! INFO Sent = ", ToSend);

	return true;
}

bool ACXZUDPBroadcaster::UDPBroadcast(FString ToSend)                 //������Ϣ����
{
	if (!IsInitSender)
		return false;

	if (!SenderSocket)
	{
		return false;
	}
	//~~~~~~~~~~~~~~~~
	//������Ϣ
	int32 BytesSent = 0;
	FString serialized = ToSend;
	TCHAR *serializedChar = serialized.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);
	if (size > 1024 * 5)
	{
		return false;
	}
//	ANSICHAR sendbuffer[10240] = { 0 };

//	ANSICHAR* sendbuffer = new ANSICHAR[size * 2];
//	memset(sendbuffer, 0, size * 2);
//	(ANSICHAR*)&sendbuffer[0] = (ANSICHAR*)TCHAR_TO_UTF8(serializedChar);

	GetUtf8Length(TCHAR_TO_UTF8(serializedChar), (int*)&size);
	
	//SenderSocket->SendTo(Writer.GetData(), Writer.Num(), BytesSent, *RemoteAddr);
	SenderSocket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteAddr);//���͸�Զ�˵�ַ

	if (BytesSent <= 0)
	{
		const FString Str = "Socket is valid but the receiver received 0 bytes, make sure it is listening properly!";
		UE_LOG(LogTemp, Error, TEXT("%s"), *Str);
		return false;
	}

	//ScreenMsg("UDP Send Succcess! INFO Sent = ", ToSend);

	return true;
}


int ACXZUDPBroadcaster::StartUDPSender(const FString & YourChosenSocketName, const FString & TheIP, const int32 ThePort)///////////��ʼ��Զ��IP ������Ϣǰ
{
	if (IsInitSender)
		return 1;
	//FIPv4Endpoint Endpoint(FIPv4Address::Any, 6789);
	//Create Remote Address.
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid;
	RemoteAddr->SetIp(*TheIP, bIsValid);
	RemoteAddr->SetPort(ThePort);

	if (!bIsValid)
	{
		return 2;
	}


	int32 SendSize = 2 * 1024 * 1024;
	SenderSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsReusable()
		.WithBroadcast()/////////////�㲥
		.WithSendBufferSize(SendSize)
		//.BoundToEndpoint(Endpoint)
		;


	//check(SenderSocket->GetSocketType() == SOCKTYPE_Datagram);

	//Set Send Buffer Size
	SenderSocket->SetSendBufferSize(SendSize, SendSize);
	SenderSocket->SetReceiveBufferSize(SendSize, SendSize);
	IsInitSender = true;
	return 0;
}

int ACXZUDPBroadcaster::StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort) // ��������ʼ��  ������Ϣǰ
{
	if (IsInitReceiver)
		return 1;
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);

	//Create Socket
	FIPv4Endpoint Endpoint(FIPv4Address::Any, ThePort);  //����ip��ַ����
														 //FIPv4Endpoint Endpoint(Addr, ThePort);                 //ָ��ip��ַ
	int32 BufferSize = 2 * 1024 * 1024;
	ReceiverSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);
	//BUFFER SIZE
	ReceiverSocket->SetSendBufferSize(BufferSize, BufferSize);
	ReceiverSocket->SetReceiveBufferSize(BufferSize, BufferSize);

	if (!ReceiverSocket)
		return 2;
	
	IsInitReceiver = true;
	return 0;
}

int ACXZUDPBroadcaster::UPDGetBroadcastData(FString& str)              //������Ϣ����
{
	if (!IsInitReceiver)
	{
		str = TEXT("");
		return 1;
	}

	if (!ReceiverSocket)
	{
		str = TEXT("");
		return 2;
	}

	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	TArray<uint8> ReceivedData;//����һ��������
	uint32 Size;

	//Ĭ��û�յ��κ���Ϣ
	str = TEXT("");
	
	if (ReceiverSocket->HasPendingData(Size))
	{
		str = "";
		int32 BytesRead = 0;

		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
		ReceiverSocket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr);//����Զ�̽��յ�ַ
		char ansiiData[1024];
		memcpy(ansiiData, ReceivedData.GetData(), BytesRead);	//�������ݵ�������
		ansiiData[BytesRead] = 0;								//�ж����ݽ���
		FString debugData = UTF8_TO_TCHAR(ansiiData);			//�ַ���ת��

		//���ж��Ƿ�ΪӲ�����͵���Ϣ,����ͬ�Ĵ���
		BYTE header[2];
		header[0] = ansiiData[0];
		header[1] = ansiiData[1];

		
		if((BYTE)ansiiData[0]==0xAA && (BYTE)ansiiData[1] == 0xAA)
		{
			//��Ӳ�����͵ı���
			//����ͷaaaa
			str = "AAAA|";
			if ((BYTE)ansiiData[2] == 0xFA && (BYTE)ansiiData[3] == 0xFA)
			{
				str.Append("FAFA|");
				if ((BYTE)ansiiData[4] == 0xC1 && (BYTE)ansiiData[5] == 0xC1)
				{
					str.Append("C1C1|");

					FString token;
					int32 intvalue;

					//�ߵͻ��仯ֵ
					intvalue = (BYTE)ansiiData[6]<<8|(BYTE)ansiiData[7];
					token = FString::FromInt(intvalue);
					str.Append(token+"|");
					//������仯ֵ
					intvalue = (BYTE)ansiiData[8] << 8 | (BYTE)ansiiData[9];
					token = FString::FromInt(intvalue);//token.FromInt(intvalue);
					str.Append(token + "|");
					//����̨�ߵ�
					intvalue = (BYTE)ansiiData[10] << 8 | (BYTE)ansiiData[11];
					token = FString::FromInt(intvalue); //token.FromInt(intvalue);
					str.Append(token + "|");
					//����̨ˮƽ
					intvalue = (BYTE)ansiiData[12] << 8 | (BYTE)ansiiData[13];
					token = FString::FromInt(intvalue); //token.FromInt(intvalue);
					str.Append(token + "|");
					//����̨_������ť �ڻ�
					if ((BYTE)ansiiData[23] & 0x4)
						str.Append("1|");
					else
						str.Append("0|");
					//�ߵͻ�����(˳1��0)
					if ((BYTE)ansiiData[24] & 0x20)
						str.Append("1|");
					else
						str.Append("0|");
					//���������(˳1��0)
					if ((BYTE)ansiiData[24] & 0x10)
						str.Append("1|");
					else
						str.Append("0|");
					//����̨_�涯��Դ����
					if ((BYTE)ansiiData[24] & 0x80)
						str.Append("1|");
					else
						str.Append("0|");
					//ǹ����
					if((BYTE)ansiiData[24] & 0x8)
						str.Append("1|");
					else
						str.Append("0|");
					//�ڼ���
					if ((BYTE)ansiiData[24] & 0x4)
						str.Append("1|");
					else
						str.Append("0|");
				}
			}
		}
		else
		{
			//��ue4���͵�
			str = UTF8_TO_TCHAR(ansiiData); //debugData;
		}
	
		// memset(ansiiData,0,1024);//��� 
	}

	return 0;
}

bool ACXZUDPBroadcaster::FakeInputBroadcast(bool bShot)                 //������Ϣ����
{
	if (!IsInitSender)
		return false;

	if (!SenderSocket)
	{
		return false;
	}
	//~~~~~~~~~~~~~~~~
	//������Ϣ
	int32 BytesSent = 0;
	//FString serialized = ToSend;
	uint8 fakebuf[35] = { 0xAA,0xAA,0xFA,0xFA,0xC1,0xC1,0x0,0x0,0x0,0x0,0x9,0x2C,0x9,0x2C,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xFF,0xFF };

	if (bShot)
		fakebuf[23] = 0x4;
	//TCHAR *serializedChar = serialized.GetCharArray().GetData();
	int32 size = 35;//FCString::Strlen(serializedChar);
	int32 sent = 0;
	//SenderSocket->SendTo(Writer.GetData(), Writer.Num(), BytesSent, *RemoteAddr);
	SenderSocket->SendTo(fakebuf, size, BytesSent, *RemoteAddr);//���͸�Զ�˵�ַ

	if (BytesSent <= 0)
	{
		const FString Str = "Socket is valid but the receiver received 0 bytes, make sure it is listening properly!";
		UE_LOG(LogTemp, Error, TEXT("%s"), *Str);
		return false;
	}

	//ScreenMsg("UDP Send Succcess! INFO Sent = ", ToSend);

	return true;
}

void ACXZUDPBroadcaster::GetUtf8Length(char const *str, int *cnt)
{

	while (*str != '\0') {
		if (*str & 1 << 7) {
			if (*str & (1 << 6)) {
				if (*str & (1 << 5)) {
					if (*str & (1 << 4)) {
						cnt[4]++, cnt[0]++, str += 4;
						continue;
					}
					cnt[3]++, cnt[0]++, str += 3;
					continue;
				}
				cnt[2]++, cnt[0]++, str += 2;
				continue;
			}
		}
		cnt[1]++, cnt[0]++, str += 1;
		continue;
	}
}