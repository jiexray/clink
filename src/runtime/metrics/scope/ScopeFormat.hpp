/**
 * This class represents the format after which the "scope" of the various
 * component metric groups is built.
 */
#pragma once

#include "StringUtils.hpp"
#include <vector>
#include <string>
#include <map>
#include <memory>

class ScopeFormat
{
private:
    std::string                 m_format;
    std::vector<std::string>    m_template;
    std::vector<int>            m_template_pos;
    std::vector<int>            m_value_pos;
protected:
    std::vector<std::string> copy_template() {
        std::vector<std::string> copy;
        for (int i = 0; i < m_template.size(); i++) {
            // copy construct
            std::string tmp_str(m_template[i]);
            copy.push_back(tmp_str);
        }
        return copy;
    }

    std::map<std::string, int> vector_to_map(const std::vector<std::string>& vec) {
        std::map<std::string, int> to_map;
        for (int i = 0; i < vec.size(); i++) {
            to_map.insert(std::make_pair(vec[i], i));
        }
        return to_map;
    }

    /* bind values to templates @prarm templates */
    void bind_variables(std::vector<std::string>& templates, std::vector<std::string>& values) {
        for (int i = 0; i < m_template_pos.size(); i++) {
            templates[m_template_pos[i]] = values[m_value_pos[i]];
        }
    }
public:
    const static std::string SCOPE_INHERIT_PARENT;
    const static std::string SCOPE_SEPARATOR;

    const static std::string SCOPE_HOST;

    const static std::string SCOPE_TASKMANAGER_ID;

    const static std::string SCOPE_JOB_ID;
    const static std::string SCOPE_JOB_NAME;

    const static std::string SCOPE_TASK_VERTEX_ID;
    const static std::string SCOPE_TASK_NAME;
    const static std::string SCOPE_TASK_SUBTASK_INDEX;
    const static std::string SCOPE_TASK_EXECUTION_ID;

    const static std::string SCOPE_OPERATOR_ID;
    const static std::string SCOPE_OPERATOR_NAME;

    ScopeFormat(std::string format, std::shared_ptr<ScopeFormat> parent, const std::vector<std::string>& variables);

    static std::string concat(char delimiter, const std::vector<std::string>& components);
};