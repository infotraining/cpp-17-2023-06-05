#ifndef GADGET_HPP
#define GADGET_HPP

class Gadget
{
    static int gen_id;

    int id_ = ++gen_id;

public:
    Gadget() = default;

    int id() const
    {
        return id_;
    }
};

#endif