import os

def rename_files(directory,renames):
    for root, dirs, files in os.walk(directory):
        for file in files:
            old_path = os.path.join(root, file)
            nufile = file
            for a,b in renames:
                nufile = nufile.replace(a,b)
            new_path = os.path.join(root, nufile)
            os.rename(old_path, new_path)
# change this if reqd
if __name__=="__main__":
    renames = (('ltage_v2','ltage_base'),('ltage_v3','ltage_nolp'),('ltage_v4','ltage_p4'),('ltage_v5','ltage_byuseful'))
    directory = './results/'

    rename_files(directory,renames)