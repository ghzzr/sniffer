// =====================================================================================
// 
//       Filename:  prototreeview.cpp
//
//    Description:  协议解析类的实现文件
//
//        Version:  1.0
//        Created:  2013年01月30日 23时06分16秒
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
//        Company:  Class 1107 of Computer Science and Technology
// 
// =====================================================================================

#include <QtGui>

#ifndef WIN32
	
#else
	#include <pcap.h>
	#define WPCAP
	#define HAVE_REMOTE 
	#include <remote-ext.h>
	#pragma warning(disable:4996)
#endif

#include "../include/prototreeview.h"
#include "../include/sniffertype.h"

ProtoTreeView::ProtoTreeView()
{
	protoInfo = NULL;

	mainModel = new QStandardItemModel;

	mainModel->setColumnCount(1);
	mainModel->setHeaderData(0, Qt::Horizontal, tr("捕获数据分析："));

	this->setModel(mainModel);
}

ProtoTreeView::~ProtoTreeView()
{
	delete protoInfo;
}

void ProtoTreeView::AnalyseProto(QByteArray *rawData)
{
	if (protoInfo != NULL) {
		delete protoInfo;
	}

	protoInfo = new AnalyseProtoType;

	//int rawLength = rawData->length();
	unsigned char *raw = (unsigned char *)rawData->data();

	eth_header		*eh;
	ip_header		*ih;
	udp_header		*uh;
	tcp_header		*th;
	unsigned short	 sport, dport;
	unsigned int	 ip_len;

	// 获得 Mac 头
	eh = (eth_header *)raw;

	QByteArray DMac, SMac;

	DMac.setRawData((const char *)eh->dstmac, 6);
	SMac.setRawData((const char *)eh->srcmac, 6);

	DMac = DMac.toHex().toUpper();
	SMac = SMac.toHex().toUpper();

	protoInfo->strDMac = protoInfo->strDMac + DMac[0] + DMac[1] + "-" + DMac[2] + DMac[3] + "-" + DMac[4] + DMac[5] + "-"
	 									    + DMac[6] + DMac[7] + "-" + DMac[8] + DMac[9] + "-" + DMac[10] + DMac[11] ;
	protoInfo->strSMac = protoInfo->strSMac + SMac[0] + DMac[1] + "-" + SMac[2] + DMac[3] + "-" + SMac[4] + DMac[5] + "-"
										    + SMac[6] + DMac[7] + "-" + SMac[8] + DMac[9] + "-" + SMac[10] + DMac[11] ;
	
	// 获得 IP 协议头
	ih = (ip_header *)(raw + 14);

	// 获得 IP 头的大小
	ip_len = (ih->ver_ihl & 0xF) * 4;

	switch (ih->proto) {
		case TCP_SIG:
			protoInfo->strTranProto = "TCP 协议";
			th = (tcp_header *)((unsigned char *)ih + ip_len);		// 获得 TCP 协议头
			sport = ntohs(th->sport);								// 获得源端口和目的端口
			dport = ntohs(th->dport);

			break;
		case UDP_SIG:
			protoInfo->strTranProto = "UDP 协议";
			uh = (udp_header *)((unsigned char *)ih + ip_len);		// 获得 UDP 协议头
			sport = ntohs(uh->sport);								// 获得源端口和目的端口
			dport = ntohs(uh->dport);

			break;
		default:
			break;
	}

	char szSaddr[24], szDaddr[24], szSPort[6], szDPort[6];

	sprintf(szSaddr, "%d.%d.%d.%d", ih->saddr[0], ih->saddr[1], ih->saddr[2], ih->saddr[3]); 
	sprintf(szDaddr, "%d.%d.%d.%d", ih->daddr[0], ih->daddr[1], ih->daddr[2], ih->daddr[3]);

	sprintf(szSPort, "%d", sport);
	sprintf(szDPort, "%d", dport);

	protoInfo->strSIP += szSaddr;
	protoInfo->strDIP += szDaddr;
	protoInfo->strSPort += szSPort;
	protoInfo->strDPort += szDPort;
}

void ProtoTreeView::ShowTreeAnalyseInfo()
{
	rebuildInfo();

	QStandardItem *item, *itemChild;
	QModelIndex index;

	item = new QStandardItem(protoInfo->strEthTitle);
	item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	mainModel->setItem(0, item);
	index = mainModel->item(0)->index();
	setExpanded(index, true);

	itemChild = new QStandardItem(protoInfo->strDMac);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);
	itemChild = new QStandardItem(protoInfo->strSMac);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);
	itemChild = new QStandardItem(protoInfo->strType);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);

	item = new QStandardItem(protoInfo->strIPTitle);
	item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	mainModel->setItem(1, item);
	index = mainModel->item(1)->index();
	setExpanded(index, true);

	itemChild = new QStandardItem(protoInfo->strVersion);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);
	itemChild = new QStandardItem(protoInfo->strHeadLength);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);
	itemChild = new QStandardItem(protoInfo->strServerType);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);
	itemChild = new QStandardItem(protoInfo->strLength);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);
	itemChild = new QStandardItem(protoInfo->strNextProto);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);
	itemChild = new QStandardItem(protoInfo->strSIP);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);
	itemChild = new QStandardItem(protoInfo->strDIP);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);

	item = new QStandardItem(protoInfo->strTranProto);
	item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	mainModel->setItem(2, item);
	index = mainModel->item(2)->index();
	setExpanded(index, true);

	itemChild = new QStandardItem(protoInfo->strSPort);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);
	itemChild = new QStandardItem(protoInfo->strDPort);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);

	item = new QStandardItem(protoInfo->strAppProto);
	item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	mainModel->setItem(3, item);
	index = mainModel->item(3)->index();
	setExpanded(index, true);

	itemChild = new QStandardItem(protoInfo->strSendInfo);
	itemChild->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	item->appendRow(itemChild);
}

void ProtoTreeView::rebuildInfo()
{
	mainModel->clear();
	
	mainModel->setColumnCount(1);
	mainModel->setHeaderData(0, Qt::Horizontal, tr("捕获数据分析："));
}
