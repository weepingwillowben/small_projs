import os
import random
rand = random.randrange

def process_all():
    read_dir = "strings/"
    write_dir = "outputs/"
    for filename in os.listdir(read_dir):
        with open(read_dir+filename) as string_file:
            orig_string = string_file.read()
            for i in range(1,5+1):
                write_filename = "{}{}{}.out".format(write_dir,filename,i)
                with open(write_filename,'w') as write_file:
                    write_file.write(process_one(orig_string))

def process_one(in_string):
    splits = []
    for split_num in range(10):
        split_start = rand(len(in_string))
        split_end = split_start+rand(3,6)
        splits.append(in_string[split_start:split_end])
    return "\n".join(splits)

process_all()
