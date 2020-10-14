#include "ScopeFormat.hpp"

const std::string ScopeFormat::SCOPE_INHERIT_PARENT = "*";
const std::string ScopeFormat::SCOPE_SEPARATOR = ".";

const std::string ScopeFormat::SCOPE_HOST = "<host>";

const std::string ScopeFormat::SCOPE_TASKMANAGER_ID = "<tm_id>";

const std::string ScopeFormat::SCOPE_JOB_ID = "<job_id>";
const std::string ScopeFormat::SCOPE_JOB_NAME = "<job_name>";

const std::string ScopeFormat::SCOPE_TASK_VERTEX_ID = "<task_id>";
const std::string ScopeFormat::SCOPE_TASK_NAME = "<task_name>";
const std::string ScopeFormat::SCOPE_TASK_SUBTASK_INDEX = "<subtask_index>";
const std::string ScopeFormat::SCOPE_TASK_EXECUTION_ID = "<task_execution_id>";

const std::string ScopeFormat::SCOPE_OPERATOR_ID = "<operator_id>";
const std::string ScopeFormat::SCOPE_OPERATOR_NAME = "<operator_name>";


ScopeFormat::ScopeFormat(std::string format, std::shared_ptr<ScopeFormat> parent, const std::vector<std::string>& variables) {
    std::vector<std::string> raw_components = StringUtils::split(format, SCOPE_SEPARATOR[0]);

    bool parent_as_prefix = (raw_components.size() > 0) && (raw_components[0] == SCOPE_INHERIT_PARENT);
    // check if begin with "*"
    if (parent_as_prefix) {
        if (parent == nullptr) {
            throw std::runtime_error("Component scope format requires parent prefix (starts with '" + SCOPE_INHERIT_PARENT + "'), but this component has no parent (is root component).");
        }            

        this->m_format = format.size() > 2 ? format.substr(2) : "<empty>";

        std::vector<std::string> parent_template = parent->m_template;

        int template_len = parent_template.size() + raw_components.size() - 1;
        for (int i = 0; i < parent_template.size(); i++) {
            std::string tmp_template(parent_template[i]);
            m_template.push_back(tmp_template);
        }
        for (int i = 1; i < raw_components.size(); i++) {
            std::string tmp_template(raw_components[i]);
            m_template.push_back(tmp_template);
        }
    } else {
        this->m_format = format.empty() ? "<empty>" : format;
        this->m_template = raw_components;
    }

    std::map<std::string, int> var_to_value_pos = vector_to_map(variables);

    for(int i = 0; i < m_template.size(); i++) {
        const std::string& component = m_template[i];

        if (component.size() >= 3 && (component[0] == '<') && (component[component.size() - 1] == '>')) {
            // this is a variable

            if (var_to_value_pos.find(component) != var_to_value_pos.end()) {
                int replacement_pos = var_to_value_pos[component];
                m_template_pos.push_back(i);
                m_value_pos.push_back(replacement_pos);
            }
        }
    }
}


std::string ScopeFormat::concat(char delimiter, const std::vector<std::string>& components) {
    std::string full_components = "";
    full_components += components[0];
    for (int i = 1; i < components.size(); i++) {
        full_components += std::to_string(delimiter);
        full_components += components[i];
    }
    return full_components;
}
