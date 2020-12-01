#pragma once

/**
 * Singleton placeholder class for state without a namespace
 */
class VoidNamespace
{
private:
    static VoidNamespace* m_instance;
    VoidNamespace() {}
public:
    static VoidNamespace* instance() {
        if (!m_instance) {
            static VoidNamespace the_instance;
            m_instance = &the_instance;
        }
        return m_instance;
    }

    ~VoidNamespace() {
        delete m_instance;
    }

    operator int() const {return 0;}
    operator double() const {return 0.0;}
};

