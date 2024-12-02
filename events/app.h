#ifndef APP_EVENT_H
#define APP_EVENT_H
#include "events.h"
namespace Lina{ namespace Events{
    template <typename FIELD>
    class AppUpdate : public Event
    {
        public:
            FIELD getFields() {return *reinterpret_cast<FIELD*>(mFields);}
            void updateField(void* oldFields, void* newFields) {}
            virtual Type getType() const override {return getStaticType();}
            static Type getStaticType() {return Type::MouseMove;}
        private:
            FIELD* mFields;
    };
}}

#endif
