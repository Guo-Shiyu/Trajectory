#pragma once

class iInit 
{
    public:
    
    // init self 
    virtual iInit*  lazy_init()      = 0;

    // on drop, default do nothing
    virtual void    destory()        {};
    virtual ~iInit()                 {};
};