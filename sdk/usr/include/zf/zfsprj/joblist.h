#pragma once
#include <mutex>

#include <Common/def.h>
SWITCH_WARNINGS_OFF
SWITCH_WARNINGS_ON

#include "ZfsPrjBasic.h"
#include "obj.h"

namespace zfprj
{

//=============================================================================================
/**
 * @brief Class holding the result messages of a job.
 */
class JobResult {
public:
    QString shortText;
    QString message;
};

//=============================================================================================
/**
 * @brief Possible states of jobs stored in the project.
 */
enum JobStatus { OPEN       =   0,
                 FINISHED   =   1,
                 FAILED     =  -1 };

class ZFJobList;

//=============================================================================================
/**
 * @brief Class to define a job for a project.
 */
class ZFSPRJ_API ZFJob : public ZFPrjObj
{
    DEFINE_ZFBASE(ZFPrjObj)

    public:
    /**
     * @brief The type of element related with this job (e.g. SCAN)
     */
    QString elementType;

    /**
     * @brief The name of the element related with this job (e.g. "scanname")
     */
    QString elementName;

    /**
     * @brief Type of the job (e.g. "PreRegister")
     */
    QString jobType;

    /**
     * @brief start time of job
     */
    QString jobStartTime;

    /**
     * @brief Current status of the job
     */
    JobStatus status;

    /**
     * @brief The result of the last exectution of the job.
     */
    JobResult result;
public:
    ZFJob(ZFJobList* p, QString elementType, QString elementName, QString jobType, QString startTime = "");
    ZFJob(ZFJobList* p);
    virtual ~ZFJob();

    std::string Xml() const override{ return PRJXML_JOB; }
};

//=============================================================================================
/**
 * @brief List of jobs of a project. This list is loaded from and written to the underlying XML document.
 */
class ZFSPRJ_API ZFJobList : public ZFPrjObjNode
{
    DEFINE_ZFBASE(ZFPrjObjNode)

private:
    /**
    * @brief Mutex to protect the job list
    */
    std::mutex jobListMutex;

public:
    ZFJobList( ZFProject* p ) : ZFPrjObjNode( p ) {};
    ~ZFJobList();

    /**
     * @brief Load the jobs from the XML file.
     *
     * @return true, if jobs where loaded, false otherwise
     */
    virtual bool Load();

    /**
     * @brief Stores the jobs to the XML file. These is not saving the XML file! This has to be done manually.
     *
     * @return true, if jobs where stored, false otherwise
     */
    virtual bool Store( double versNr );

    QString Typ() const override { return QObject::tr("Jobs"); }
    std::string Xml() const override { return PRJXML_JOBS; }

    /**
     * @brief Add a ZFJob to the list.
     *
     * @param job the pointer to the job.
     */
    void addJobToList( ZFJob* job );

    /**
     * @brief Lookup the job with the given key elements.
     *
     * @param elementType   type of the related element
     * @param elementName   name of the related element
     * @param jobType       job type of the related job
     *
     * @return the pointer to the job or NULL, if no job is matching the combined key
     */
    ZFJob* getJob( QString elementType, QString elementName, QString jobType );

    /**
     * @brief Get the list of open jobs.
     *
     * @param jobList the list of open jobs (out)
     */
    void getOpenJobs(ZFJobList& jobList);

    /**
     * @brief Get the list of failed jobs.
     *
     * @param jobList the list of failed jobs (out)
     */
    void getFailedJobs(ZFJobList& jobList);

};
//=============================================================================================

/**
* Type for a list of ZFJob pointers.
*/
typedef std::list<ZFJob*> JobListType;

} // end namespace zfprj
