/*
     This file is part of HIDWebServer
     (C) Riccardo Ventrella
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 3.0 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <stdio.h>
#include <memory.h>
#include "CHidApi.h"

#define RFIDREADER_VID	0//0x8ff
#define RFIDREADER_PID	0//0x009


// Uncommented this below to dump communication on console
//#define __DUMP__

bool CHidApi::ReadReport(void)
{
	int res;
	
	if ( !m_pHandle )
		return false;
	this->ResetRxData();
#ifdef __DUMP__
	fprintf(stdout, "ReadReport before hid_read-> m_pHandle = %p\n", m_pHandle);
#endif
	// Since the RX buffer is quite wide, let's read directly within it
	res = hid_read(m_pHandle, m_USB_RxData, sizeof(m_USB_RxData));
#ifdef __DUMP__
	fprintf(stdout, "ReadReport -> m_pHandle = %p, res = %d\n", m_pHandle, res);
	int i;
	for ( i=0; i<res; ++i )
		fprintf(stdout, " %x", m_USB_RxData[i]);
	fprintf(stdout, "\n");
#endif	
	
	return ( res > 0 );
}

size_t CHidApi::FindRFIDReadersHids(vector<hid_device*>& HidHandles)
{
	hid_device_info *pHeadDev, *pCurrDev;
	hid_device		*pHandle;
	
	HidHandles.clear();
	
	pHeadDev = hid_enumerate(RFIDREADER_VID, RFIDREADER_PID);
	
	//#ifdef __DUMP__
	fprintf(stdout, "FindRFIDReadersHids -> head = %p\n", pHeadDev);
	//#endif
	
	pCurrDev = pHeadDev;
	// Loop on the linked list until the pointer is valid
	while ( pCurrDev )
	{
		//#ifdef __DUMP__
	// Skip vendor_id and product_id on Mac
		if ( pCurrDev->vendor_id != 0x5ac || pCurrDev->product_id != 0x262 )
		{ 
			fwprintf(stdout, L"VendorId = %x, ProductId = %x, Serial = %s\n", pCurrDev->vendor_id, pCurrDev->product_id, pCurrDev->serial_number);
		//#endif
		// Try to open the device and assign its pointer
		pHandle = hid_open_path(pCurrDev->path);
		if ( pHandle )
		{
			//#ifdef __DUMP__
			printf("%p: Open successful\n", pHandle);
			//#endif
			// Listen to incoming messages on this channel
			//while (Handle._ReadReport());			
			HidHandles.push_back(pHandle);
		}
		else
			printf("Open failed\n");
		}
		// Get next device
		pCurrDev = pCurrDev->next;
	}
	hid_free_enumeration(pHeadDev);
	
	return HidHandles.size();
}
