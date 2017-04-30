#include <stdio.h>
#include <iostream>
#include <gtk/gtk.h>
#include <keybinder.h>
#include <node.h>
#include <string>
#include <thread>
#include <uv.h>
#include <map>
#include <mutex>
#include <condition_variable>
#include <deque>

using namespace v8;
using namespace node;
using namespace std;

template <typename T>
class queue
{
private:
    std::mutex d_mutex;
    std::condition_variable d_condition;
    std::deque<T> d_queue;
public:
  void push(T const& value) {
    {
      std::unique_lock<std::mutex> lock(this->d_mutex);
      d_queue.push_front(value);
    }
    this->d_condition.notify_one();
  }
  T pop() {
    std::unique_lock<std::mutex> lock(this->d_mutex);
    this->d_condition.wait(lock, [=]{ return !this->d_queue.empty(); });
    T rc(std::move(this->d_queue.back()));
    this->d_queue.pop_back();
    return rc;
  }
};
struct Baton
{
    uv_work_t request;
    string key;
};

thread *listen_th=NULL;
queue<string> keys;
map<string,Persistent<Function>*> callbacks;


void handler (const char *keystring, void *user_data) {
  keys.push(string(keystring));
}
void listener(){
  gtk_main();
}


void on(const FunctionCallbackInfo<Value>& args){
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  auto key =string(*String::Utf8Value(args[0].As<String>()));
  keybinder_bind(key.c_str(),handler,NULL);
  Local<Function> callback = args[1].As<Function>();
  Persistent<Function> *p=new Persistent<Function>();
  p->Reset(isolate,callback);
  callbacks[key]= p;//Persistent<Function>(isolate,callback)
}
void wait_key (uv_work_t* req){
  Baton* baton = static_cast<Baton*>(req->data);
  baton->key = keys.pop();
}
void call(uv_work_t* req,int){
  Baton* baton = static_cast<Baton*>(req->data);
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);
  callbacks[baton->key]->Get(isolate)->Call(isolate->GetCurrentContext()->Global(),0,NULL);
  uv_queue_work(uv_default_loop(), &baton->request, wait_key, call);  
}

void start(const FunctionCallbackInfo<Value>& args){
  if(!listen_th){
   listen_th=new thread(listener);
  }
}

void init(Handle<Object> exports) 
{
  gtk_init(0,NULL);
  keybinder_init();
  Isolate* isolate = Isolate::GetCurrent();
  exports->Set(String::NewFromUtf8(isolate, "on"),
      FunctionTemplate::New(isolate, on)->GetFunction());
  
  exports->Set(String::NewFromUtf8(isolate, "start"),
      FunctionTemplate::New(isolate, start)->GetFunction()); 
  Baton* baton = new Baton();
  baton->request.data = baton;
  uv_queue_work(uv_default_loop(), &baton->request, wait_key, call);
}
NODE_MODULE(gui, init)