#ifndef GADGET_HPP
#define GADGET_HPP

class Gadget
{
    inline static int gen_id = 0; // definition

    int id_ = ++gen_id;

public:
    Gadget() = default;

    int id() const
    {
        return id_;
    }
};

#endif