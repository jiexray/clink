/**
 * Identifies InputChannel in a given subtask.
 */
#pragma once
#include <string>

class InputChannelInfo
{
private:
    int         m_gate_idx;
    int         m_input_channel_idx;
public:
    InputChannelInfo(int gate_idx, int input_channel_idx): m_gate_idx(gate_idx), m_input_channel_idx(input_channel_idx){};

    /* Properties */
    int         get_gate_idx() {return m_gate_idx;}
    int         get_input_channel_idx() {return m_input_channel_idx;}

    std::string to_string() {return "InputChannelInfo{gate_idx=" + std::to_string(m_gate_idx) +
                                    ", input_channel_idx=" + std::to_string(m_input_channel_idx) + "}";}
};

