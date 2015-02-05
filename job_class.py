''' This is a file to define the class of all jobs,
    You have to modify this file if you want to add new type of jobs.'''
import sys, os, random, re, copy

def get_current_PID(Type):
    workspace=os.path.abspath(".")
    KeyWord=Type+"_para"
    filelist=sorted([int(e.split('_')[0]) for e in os.listdir(workspace) if KeyWord in e])
    if len(filelist)==0:
        NextPID=0
    else:
        NextPID=filelist[-1]+1
    return filelist, NextPID

#base class of all jobs
class Job:
    '''Class Job is the base class of all job objects.
       You have to add new subclass of Job if you want to
       create new type of job.'''
    def __init__(self, para):
        # interesting point here, python will actually call the 
        #__check_parameters__ of subclass,
        # So you don't have to call __check_parameters__ 
        #in subclass initialization again!
        if self.__check_parameters__(para) is False:
            print "Something is wrong with the inlist! Abandon!"
            sys.exit()
        self.control=para.pop("Control")
        self.job=para.pop("Job")
        self.pid = []
        self.para = para

    def to_dict(self):
        '''output the corresponding string of the job class'''
        pid=self.pid.pop(0)
        self.control["__Type"]=self.job["Type"]
        self.job["WeightFile"]="Weight"
        self.job["MessageFile"]="Message"
        self.job["OutputFile"]="Dyson_Output"
        self.job["PID"] = pid
        self.__set_model_specific__()
        return pid, {"Job": copy.deepcopy(self.job), "Para":copy.deepcopy(self.para)}

    def __check_parameters__(self, para):
        if para["Control"]["__Execute"] is "":
            print "Please specify the executive file name!"
            return False

    def __set_model_specific__(self):
        PI=3.141592653589793238
        if self.para["Model"]["Name"] in ("J1J2", "Heisenberg"):
            self.para["Model"]["Hopping"]=[0.0,]
            mu=1j*PI/2.0/self.para["Tau"]["Beta"]
            self.para["Model"]["ChemicalPotential"]=[mu,mu]

class JobMonteCarlo(Job):
    '''job subclass for monte carlo jobs'''
    def __init__(self, para):
        Job.__init__(self, para)
        self.job["Type"] = "MC"
        #search folder for old jobs, the new pid=largest old pid+1
        PIDList, NextPID=get_current_PID(self.job["Type"])
        if self.job["DoesLoad"] and len(PIDList) is not 0:
            self.pid=PIDList[:self.control["__Duplicate"]]
        else:
            self.pid=range(NextPID, NextPID+self.control["__Duplicate"])

    def __check_parameters__(self, para):
        if Job.__check_parameters__(self, para) is False:
            return False
        if type(para["Markov"]["OrderReWeight"]) is not list:
            print "The Reweight should be a list!"
            return False
        if para["Markov"]["Order"]+1>len(para["Markov"]["OrderReWeight"]):
            print "The Reweight numbers should be equal/larger than Order!"
            return False

    def to_dict(self):
        pid, Dict=Job.to_dict(self)
        if self.job["DoesLoad"]:
            Dict.pop("Para")
        else:
        #set Seed here so that each job has it own rng seed
            Dict["Para"]["Markov"]["Seed"] = int(random.random()*2**30)
        return pid, Dict

class JobDyson(Job):
    '''job subclass for self consistent loop jobs'''
    def __init__(self, para):
        Job.__init__(self, para)
        self.job["Type"] = "DYSON"
        PIDList, NextPID=get_current_PID(self.job["Type"])
        if self.control["__Duplicate"]>0:
            self.pid=range(1)
        #self.pid=range(NextPID, NextPID+self.control["__Duplicate"])

    def to_dict(self):
        return Job.to_dict(self)

