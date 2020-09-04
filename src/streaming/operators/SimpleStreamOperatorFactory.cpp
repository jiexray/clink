#include "SimpleStreamOperatorFactory.hpp"

template <class OUT>
std::shared_ptr<SimpleStreamOperatorFactory<OUT>> SimpleStreamOperatorFactory<OUT>::of(std::shared_ptr<StreamOperator<OUT>> op) {
    if (op == nullptr) {
        return nullptr;
    } else if (std::dynamic_pointer_cast<AbstractUdfStreamOperator<Function, OUT>>(op)) {
        // The operator is a sub-class of AbstractUdfStreamOperator
        // std::cout << "[DEBUG] SimpleStreamOperatorFactory<OUT>::of(): is AbstractUdfStreamOperator" << std::endl;
        std::shared_ptr<AbstractUdfStreamOperator<Function, OUT>> udf_operator = std::dynamic_pointer_cast<AbstractUdfStreamOperator<Function, OUT>>(op);
        return std::make_shared<SimpleUdfStreamOperatorFactory<OUT>>(udf_operator);
    }
    std::cout << "Unknown Stream Operator Type" << std::endl;
    throw std::runtime_error("Unknown Stream Operator Type");
    return nullptr;
}



// template class SimpleStreamOperatorFactory<int>;
// template class SimpleStreamOperatorFactory<double>;
template class SimpleStreamOperatorFactory<std::string>;