
extern "C" {
#include "message.h"
}
#include <gtest/gtest.h>


TEST (Message, message)
{
	message msg;
	std::string ip;
	ip =  "192.168.0.60";
	std::string message;
	message = "This is a test";

	message_fill(&msg, SOURCE_SERVER, TYPE_ACK, ip.c_str(), 5067, 256, 780, message.c_str());

	ASSERT_EQ(msg.header.source ,SOURCE_SERVER);
	ASSERT_EQ(msg.header.req_type, TYPE_ACK);	
	ASSERT_STREQ((const char*)msg.header.ip_addr, ip.c_str());
	ASSERT_EQ(msg.header.port_addr, 5067);	
	ASSERT_EQ(msg.header.job_id, 780);	
	ASSERT_EQ(msg.header.memory,256);	
	ASSERT_STREQ((const char*)msg.text, message.c_str());
	ASSERT_EQ(msg.header.length,message.length());	
		

}
TEST(test_group, test1)
{
	ASSERT_EQ(1,1);
}
