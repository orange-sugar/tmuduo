#include "tmuduo/net/Buffer.h"

#include <gtest/gtest.h>

using namespace tmuduo::net;
using std::string;

TEST(testBufferAppendRetrieve, BasicAssertions)
{
  Buffer buf;
  EXPECT_EQ(buf.readableBytes(), 0);
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize);
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend);

  const string str(200, 'x');
  buf.append(str);
  EXPECT_EQ(buf.readableBytes(), str.size());
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize - str.size());
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend);

  const string str2 = buf.retrieveAsString(50);
  EXPECT_EQ(str2.size(), 50);
  EXPECT_EQ(buf.readableBytes(), str.size() - str2.size());
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize - str.size());
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend + str2.size());
  EXPECT_EQ(str2, string(50, 'x'));

  buf.append(str);
  EXPECT_EQ(buf.readableBytes(), 2*str.size() - str2.size());
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize - 2*str.size());
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend + str2.size());

  const string str3 =  buf.retrieveAllAsString();
  EXPECT_EQ(str3.size(), 350);
  EXPECT_EQ(buf.readableBytes(), 0);
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize);
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend);
  EXPECT_EQ(str3, string(350, 'x'));
}

TEST(testBufferGrow, BasicAssertions)
{
  Buffer buf;
  buf.append(string(400, 'y'));
  EXPECT_EQ(buf.readableBytes(), 400);
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-400);

  buf.retrieve(50);
  EXPECT_EQ(buf.readableBytes(), 350);
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-400);
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend+50);

  buf.append(string(1000, 'z'));
  EXPECT_EQ(buf.readableBytes(), 1350);
  EXPECT_EQ(buf.writableBytes(), 0);
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend+50); // FIXME

  buf.retrieveAll();
  EXPECT_EQ(buf.readableBytes(), 0);
  EXPECT_EQ(buf.writableBytes(), 1400); // FIXME
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend);
}

TEST(testBufferInsideGrow, BasicAssertions)
{
  Buffer buf;
  buf.append(string(800, 'y'));
  EXPECT_EQ(buf.readableBytes(), 800);
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-800);

  buf.retrieve(500);
  EXPECT_EQ(buf.readableBytes(), 300);
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-800);
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend+500);

  buf.append(string(300, 'z'));
  EXPECT_EQ(buf.readableBytes(), 600);
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-600);
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend);
}

TEST(testBufferPrepend, BasicAssertions)
{
  Buffer buf;
  buf.append(string(200, 'y'));
  EXPECT_EQ(buf.readableBytes(), 200);
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-200);
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend);

  int x = 0;
  buf.prepend(&x, sizeof x);
  EXPECT_EQ(buf.readableBytes(), 204);
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize-200);
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend - 4);
}

TEST(testBufferReadInt, BasicAssertions)
{
  Buffer buf;
  buf.append("HTTP");
  EXPECT_EQ(buf.readableBytes(), 4);
  EXPECT_EQ(buf.peekInt8(), 'H');
  int top16 = buf.peekInt16();
  EXPECT_EQ(top16, 'H'*256 + 'T');
  EXPECT_EQ(buf.peekInt32(), top16*65536 + 'T'*256 + 'P');

  EXPECT_EQ(buf.readInt8(), 'H');
  EXPECT_EQ(buf.readInt16(), 'T'*256 + 'T');
  EXPECT_EQ(buf.readInt8(), 'P');
  EXPECT_EQ(buf.readableBytes(), 0);
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize);
  
  buf.appendInt8(-1);
  buf.appendInt16(-2);
  buf.appendInt32(-3);
  EXPECT_EQ(buf.readableBytes(), 7);
  EXPECT_EQ(buf.readInt8(), -1);
  EXPECT_EQ(buf.readInt16(), -2);
  EXPECT_EQ(buf.readInt32(), -3);
}

TEST(testBufferFindEOL, BasicAssertions)
{
  Buffer buf;
  buf.append(string(100000, 'x'));
  const char* null = NULL;
  EXPECT_EQ(buf.findEOL(), null);
  EXPECT_EQ(buf.findEOL(buf.peek()+90000), null);
}

void output(Buffer&& buf, const void* inner)
{
  Buffer newbuf(std::move(buf));
  // printf("New Buffer at %p, inner %p\n", &newbuf, newbuf.peek());
  EXPECT_EQ(inner, newbuf.peek());
}

// NOTE: This test fails in g++ 4.4, passes in g++ 4.6.
TEST(testMove, test)
{
  Buffer buf;
  buf.append("tmuduo", 6);
  const void* inner = buf.peek();
  // printf("Buffer at %p, inner %p\n", &buf, inner);
  output(std::move(buf), inner);
}