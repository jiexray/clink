/**
 * Deployment descriptor for a result partition.
 */
#pragma once

class ResultPartitionDeploymentDescriptor
{
private:
    int m_number_of_subpartitions;
public:
    ResultPartitionDeploymentDescriptor(int);
    ~ResultPartitionDeploymentDescriptor() {};

    /* Properties */
    int get_number_of_subpartitions() {return m_number_of_subpartitions;}
};

inline ResultPartitionDeploymentDescriptor::ResultPartitionDeploymentDescriptor(int number_of_subpartitions):
m_number_of_subpartitions(number_of_subpartitions){}

