#pragma once

namespace test {

    class case_object {
    public:
        virtual ~case_object();

        virtual void run() = 0;

        virtual void setup() = 0;
        virtual void teardown() = 0;
    };

}
