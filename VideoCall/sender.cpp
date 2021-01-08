#include "sender.h"


vc::Sender::Sender(string target_url, string target_port):target_url(target_url),target_port(target_port)
{
        
}

void vc::Sender::format_ctx_set()
{
    format_ctx = avformat_alloc_context();
    
    
}

void vc::Sender::create_decoder()
{
    
}
