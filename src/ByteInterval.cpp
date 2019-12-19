//===- ByteInterval.cpp -----------------------------------------*- C++ -*-===//
//
//  Copyright (C) 2018 GrammaTech, Inc.
//
//  This code is licensed under the MIT license. See the LICENSE file in the
//  project root for license terms.
//
//  This project is sponsored by the Office of Naval Research, One Liberty
//  Center, 875 N. Randolph Street, Arlington, VA 22203 under contract #
//  N68335-17-C-0700.  The content of the information does not necessarily
//  reflect the position or policy of the Government and no official
//  endorsement should be inferred.
//
//===----------------------------------------------------------------------===//
#include <gtirb/ByteInterval.hpp>
#include <gtirb/Serialization.hpp>
#include <proto/ByteInterval.pb.h>
#include <iterator>

using namespace gtirb;

void ByteInterval::toProtobuf(MessageType* Message) const {
  nodeUUIDToBytes(this, *Message->mutable_uuid());

  if (Address.has_value()) {
    Message->set_has_address(true);
    Message->set_address((uint64_t)*Address);
  } else {
    Message->set_has_address(false);
  }

  Message->set_size(getSize());
  auto BytesIt = Bytes.begin<char>();
  Message->mutable_contents()->reserve(AllocatedSize);
  std::copy(BytesIt, BytesIt + AllocatedSize,
            std::back_inserter(*Message->mutable_contents()));

  for (const auto& N : this->blocks()) {
    auto* ProtoBlock = Message->add_blocks();

    switch (N.getKind()) {
    case Node::Kind::CodeBlock: {
      auto& B = cast<CodeBlock>(N);
      ProtoBlock->set_offset(B.getOffset());
      B.toProtobuf(ProtoBlock->mutable_code());
    } break;
    case Node::Kind::DataBlock: {
      auto& B = cast<DataBlock>(N);
      ProtoBlock->set_offset(B.getOffset());
      B.toProtobuf(ProtoBlock->mutable_data());
    } break;
    default: { assert(!"unknown Node::Kind in ByteInterval::toProtobuf"); }
    }
  }

  auto& ProtoSymExpr = *Message->mutable_symbolic_expressions();
  for (const auto& Pair : this->symbolic_expressions()) {
    ProtoSymExpr[Pair.first] = gtirb::toProtobuf(Pair.second);
  }
}

ByteInterval* ByteInterval::fromProtobuf(Context& C, Section* Parent,
                                         const MessageType& Message) {
  std::optional<Addr> A;
  if (Message.has_address()) {
    A = Addr(Message.address());
  }

  ByteInterval* BI = ByteInterval::Create(
      C, Parent, A, Message.contents().begin(), Message.contents().end(),
      Message.size(), Message.contents().size());

  setNodeUUIDFromBytes(BI, Message.uuid());

  for (const auto& ProtoBlock : Message.blocks()) {
    switch (ProtoBlock.value_case()) {
    case proto::Block::ValueCase::kCode: {
      CodeBlock* node = CodeBlock::fromProtobuf(C, BI, ProtoBlock.code());
      BI->Blocks.emplace(ProtoBlock.offset(), node);
    } break;
    case proto::Block::ValueCase::kData: {
      DataBlock* node = DataBlock::fromProtobuf(C, BI, ProtoBlock.data());
      BI->Blocks.emplace(ProtoBlock.offset(), node);
    } break;
    default: {
      assert(!"unknown Block::ValueCase in ByteInterval::fromProtobuf");
    }
    }
  }
  return BI;
}

void ByteInterval::symbolicExpressionsFromProtobuf(Context& C,
                                                   const MessageType& Message) {
  mutateModuleIndices(this, [&]() {
    for (const auto& Pair : Message.symbolic_expressions()) {
      SymbolicExpression SymExpr;
      gtirb::fromProtobuf(C, SymExpr, Pair.second);
      SymbolicExpressions[Pair.first] = SymExpr;
    }
  });
}

ByteVector& gtirb::getByteVector(ByteInterval* BI) { return BI->Bytes; }
const ByteVector& gtirb::getByteVector(const ByteInterval* BI) {
  return BI->Bytes;
}
