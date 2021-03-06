/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_CORE_DISTRIBUTED_RUNTIME_EAGER_EAGER_CLIENT_H_
#define TENSORFLOW_CORE_DISTRIBUTED_RUNTIME_EAGER_EAGER_CLIENT_H_

#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/protobuf/eager_service.pb.h"

namespace tensorflow {
namespace eager {

// This is a base class that can be implemented by a variety of
// transports (e.g. gRPC which for each of the client methods makes an RPC).
class EagerClient {
 public:
  virtual ~EagerClient() {}
#define CLIENT_METHOD(method)                                \
  virtual void method##Async(const method##Request* request, \
                             method##Response* response,     \
                             StatusCallback done) = 0;

  CLIENT_METHOD(CreateContext);
  CLIENT_METHOD(Enqueue);
  CLIENT_METHOD(WaitQueueDone);
  CLIENT_METHOD(KeepAlive);
  CLIENT_METHOD(CloseContext);
  CLIENT_METHOD(RegisterFunction);

#undef CLIENT_METHOD

  // Feeds `request` into the request stream of EagerService::StreamingEnqueue.
  // `response` will be filled with the response for this `request`. The
  // 1-to-1 correspondence between requests and responses is a property
  // of the current service implementation. When the response is received,
  // `done` is invoked with the current status of the StreamingEnqueue call.
  // The status can contain an error because of an earlier request in the
  // current streaming call.
  // The client initiates a streaming call the first time StreamingEnqueueAsync
  // is invoked and keeps it open until some error condition.
  // Similarly to the methods above, the request can be deleted as soon as
  // StreamingEnqueueAsync returns.
  virtual Status StreamingEnqueueAsync(const EnqueueRequest* request,
                                       EnqueueResponse* response,
                                       StatusCallback done) = 0;
};

// Simple wrapper class that can be used to retrieve EagerClients.
class EagerClientCache {
 public:
  virtual ~EagerClientCache() {}
  virtual Status GetClient(const string& target, EagerClient** client) = 0;
};

}  // namespace eager
}  // namespace tensorflow

#endif  // TENSORFLOW_CORE_DISTRIBUTED_RUNTIME_EAGER_EAGER_CLIENT_H_
