#pragma once

struct IHomework
{
    virtual ~IHomework() = default;

    virtual void init() = 0;
    virtual void render() = 0;
};