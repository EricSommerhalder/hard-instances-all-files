from lab.experiment import Experiment
from lab.environments import BaselSlurmEnvironment
import os
ENV = BaselSlurmEnvironment(partition="infai_1", email="eric.sommerhalder@unibas.ch", memory_per_cpu="6354M")
DIR = os.path.dirname(os.path.abspath(__file__))
exp = Experiment(environment=ENV)
exp.add_resource("solver", os.path.join(DIR, "hardInstanceSearch")) 
for i in [13, 16, 21, 48, 55, 59, 81, 98]:
    run = exp.add_run()
    run.add_resource("PDB_1", os.path.join(DIR, "STP(4,4)-0-0;11;12;13;14;15-8bpe-lex.pdb"), symlink=True)
    run.add_resource("PDB_2", os.path.join(DIR, "STP(4,4)-0-0;1;2;3;4;5;6;7-8bpe-lex.pdb"), symlink=True)
    run.add_resource("PDB_3", os.path.join(DIR, "STP(4,4)-0-0;8;9;12;13-8bpe-lex.pdb"), symlink=True)
    run.add_command("solve", ["{solver}", str(i)], 1800, 6354)
    run.set_property("id", [str(i)])
exp.add_step("build", exp.build)
exp.add_step("start", exp.start_runs)
exp.add_fetcher(name="fetch")
exp.run_steps()
