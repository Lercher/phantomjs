#include "msmqsender.h"
#include "windows.h"
#include "Mq.h"
#include <iostream>

#pragma comment(lib,"mqrt.lib")

// Implementation adapted from the example in http://msdn.microsoft.com/en-us/library/windows/desktop/ms701483(v=vs.85).aspx
MSMQSender::MSMQSender()
{
}


QString MSMQSender::SendUtf8String(const QString& messagequeue, const QString& label, const QString& content, const bool transactional) const
{
    if (messagequeue.length() > 255) return " MSMQSender::SendUtf8String messagequeue name too long";
    if (label.length() > 255) return "MSMQSender::SendUtf8String label name too long";

    wchar_t mqname[256];
    messagequeue.toWCharArray(mqname); mqname[messagequeue.length()] = 0; // we need to keep the bytes

    wchar_t mlabel[256];
    label.toWCharArray(mlabel); mlabel[label.length()] = 0;

    QByteArray contentencoded = content.toUtf8(); // we need to keep the bytes

    DWORD cPropId = 0;                                  // Property counter
    HRESULT hr = MQ_OK;                                 // Return code
    HANDLE hQueue = NULL;                               // Queue handle

    // Define an MQMSGPROPS structure.
    const int NUMBEROFPROPERTIES = 4;                   // Number of properties
    MQMSGPROPS msgProps;
    MSGPROPID aMsgPropId[NUMBEROFPROPERTIES];
    MQPROPVARIANT aMsgPropVar[NUMBEROFPROPERTIES]; // http://msdn.microsoft.com/en-us/library/windows/desktop/aa380072(v=vs.85).aspx
    HRESULT aMsgStatus[NUMBEROFPROPERTIES];

    // Specify the message properties to be sent.
    Q_ASSERT(cPropId < NUMBEROFPROPERTIES-1);
    aMsgPropId[cPropId] = PROPID_M_LABEL;               // Property ID
    aMsgPropVar[cPropId].vt = VT_LPWSTR;                // Type indicator
    aMsgPropVar[cPropId].pwszVal = mlabel; // The message label
    aMsgStatus[cPropId] = 0;
    cPropId++;

    CAUB ct; // The Message Content String as UTF-8
    ct.cElems = contentencoded.size();
    ct.pElems = (UCHAR *)contentencoded.constData();
    Q_ASSERT(cPropId < NUMBEROFPROPERTIES-1);
    aMsgPropId[cPropId] = PROPID_M_BODY;                // Property ID
    aMsgPropVar[cPropId].vt = VT_UI1|VT_VECTOR;         // Type indicator
    aMsgPropVar[cPropId].caub = ct;
    aMsgStatus[cPropId] = 0;
    cPropId++;

    Q_ASSERT(cPropId < NUMBEROFPROPERTIES-1);
    aMsgPropId[cPropId] = PROPID_M_BODY_TYPE;           // Property ID
    aMsgPropVar[cPropId].vt = VT_UI4;                   // Type indicator
    aMsgPropVar[cPropId].ulVal = VT_UI1|VT_VECTOR;      // as a byte array
    aMsgStatus[cPropId] = 0;
    cPropId++;

    Q_ASSERT(cPropId < NUMBEROFPROPERTIES-1);
    aMsgPropId[cPropId] = PROPID_M_JOURNAL;           // Property ID
    aMsgPropVar[cPropId].vt = VT_UI1;                 // Type indicator
    aMsgPropVar[cPropId].ulVal = MQMSG_JOURNAL|MQMSG_DEADLETTER;      // keep messages in journal + keep messages thet dont reach their destination in dead letter queue
    aMsgStatus[cPropId] = 0;
    cPropId++;


    // Initialize the MQMSGPROPS structure.
    msgProps.cProp = cPropId;
    msgProps.aPropID = aMsgPropId;
    msgProps.aPropVar = aMsgPropVar;
    msgProps.aStatus = aMsgStatus;

    //std::cout << "MQOpenQueue " << messagequeue.toAscii().constData() << std::endl;

    // Call MQOpenQueue to open the queue with send access.
    hr = MQOpenQueue(
                     mqname,                            // Format name of the queue
                     MQ_SEND_ACCESS,                    // Access mode
                     MQ_DENY_NONE,                      // Share mode
                     &hQueue                            // OUT: Queue handle
                     );

    // Handle any error returned by MQOpenQueue.
    if (FAILED(hr))
        return QString("MQOpenQueue(%1) failed with HRESULT=0x%2. See http://msdn.microsoft.com/en-us/library/windows/desktop/ms699817(v=vs.85).aspx").arg(messagequeue).arg(QString::number(hr, 16));


    // std::cout << "MQSendMessage " << messagequeue.toAscii().constData() << " hQ" << hQueue << std::endl;
    // Call MQSendMessage to send the message to the queue.
    ITransaction* tr = transactional ? MQ_SINGLE_MESSAGE : MQ_NO_TRANSACTION;
    hr = MQSendMessage(
                       hQueue,      // Queue handle
                       &msgProps,   // Message property structure
                       tr           // MQ_NO_TRANSACTION or MQ_SINGLE_MESSAGE
                       );
    if (hr == MQ_INFORMATION_PROPERTY /*0x400E0001*/)
    {
        std::cout << "HRESULT " << hr << " - One or more of the properties resulted in a warning even though the function completed." << std::endl;
        for(unsigned int i=0; i<cPropId; ++i)
            std::cout << "PROP" << i << " - HRESULT " << aMsgStatus[i] << std::endl;
    }
    if (FAILED(hr))
    {
      MQCloseQueue(hQueue);
      return QString("MQSendMessage(%1) failed with HRESULT=0x%2. See http://msdn.microsoft.com/en-us/library/windows/desktop/ms711382(v=vs.85).aspx").arg(messagequeue).arg(QString::number(hr, 16));
    }

    // Call MQCloseQueue to close the queue.
    hr = MQCloseQueue(hQueue); // we dont care if close() failed

    return "OK";
}


/*
HRESULT MSMQSender::SendMessage(
                    WCHAR * wszQueueName,
                    WCHAR * wszComputerName
                    )
{

  // Validate the input strings.
  if (wszQueueName == NULL || wszComputerName == NULL)
  {
    return MQ_ERROR_INVALID_PARAMETER;
  }


  // Define the required constants and variables.
  const int NUMBEROFPROPERTIES = 5;                   // Number of properties
  DWORD cPropId = 0;                                  // Property counter
  HRESULT hr = MQ_OK;                                 // Return code
  HANDLE hQueue = NULL;                               // Queue handle

  // Define an MQMSGPROPS structure.
  MQMSGPROPS msgProps;
  MSGPROPID aMsgPropId[NUMBEROFPROPERTIES];
  MQPROPVARIANT aMsgPropVar[NUMBEROFPROPERTIES];
  HRESULT aMsgStatus[NUMBEROFPROPERTIES];


  // Specify the message properties to be sent.
  aMsgPropId[cPropId] = PROPID_M_LABEL;               // Property ID
  aMsgPropVar[cPropId].vt = VT_LPWSTR;                // Type indicator
  aMsgPropVar[cPropId].pwszVal = L"Test Message";     // The message label
  cPropId++;


  // Initialize the MQMSGPROPS structure.
  msgProps.cProp = cPropId;
  msgProps.aPropID = aMsgPropId;
  msgProps.aPropVar = aMsgPropVar;
  msgProps.aStatus = aMsgStatus;


  // Create a direct format name for the queue.
  WCHAR * wszFormatName = NULL;
  DWORD dwBufferLength = 0;
  const WCHAR * wszFormatStr = L"DIRECT=OS:%s\\%s" ;
  dwBufferLength = wcslen(wszQueueName) + wcslen(wszComputerName) +
                                 wcslen(wszFormatStr) - 4;

  wszFormatName = new WCHAR[dwBufferLength];
  if (wszFormatName == NULL)
  {
    return MQ_ERROR_INSUFFICIENT_RESOURCES;
  }
  memset(wszFormatName, 0, dwBufferLength*sizeof(WCHAR));

  if (_snwprintf_s(
                 wszFormatName,
                 dwBufferLength,
                 dwBufferLength - 1,
                 L"DIRECT=OS:%s\\%s",
                 wszComputerName,
                 wszQueueName
                ) < 0)
  {
    std::cerr << "The format name is too long for the buffer specified." << std::endl;
    return FALSE;
  }
  else
  {
    wszFormatName[dwBufferLength - 1] = L'\0';
  }

  // Call MQOpenQueue to open the queue with send access.
  hr = MQOpenQueue(
                   wszFormatName,                     // Format name of the queue
                   MQ_SEND_ACCESS,                    // Access mode
                   MQ_DENY_NONE,                      // Share mode
                   &hQueue                            // OUT: Queue handle
                   );
  // Free the memory that was allocated for the format name string.
  delete [] wszFormatName;


  // Handle any error returned by MQOpenQueue.
  if (FAILED(hr))
  {
    return hr;
  }


  // Call MQSendMessage to send the message to the queue.
  hr = MQSendMessage(
                     hQueue,                          // Queue handle
                     &msgProps,                       // Message property structure
                     MQ_NO_TRANSACTION               // Not in a transaction
                     );
  if (FAILED(hr))
  {
    MQCloseQueue(hQueue);
    return hr;
  }


  // Call MQCloseQueue to close the queue.
  hr = MQCloseQueue(hQueue);

  return hr;
}
*/
