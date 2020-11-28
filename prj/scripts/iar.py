#!/usr/bin/python
import os
import sys
import string
import shutil
import re

import xml.etree.ElementTree as etree
import project_cfg
from xml.etree.ElementTree import SubElement
from os.path import basename
from xml_format import gen_indent
from project_cfg import Projects

workspace = "nrf24l01"

OPT_DIR = '$PROJ_DIR$/opts/'

element_dict = {
    "OGChipSelectEditMenu": "",
    "IlinkOutputFile": "",
    "IlinkIcfFile": "",
}


def convert_file_name(filename):
    """get the right path for relative path"""
    '''if not os.path.isabs(filename):
        filename = os.path.abspath('../../' + filename)'''
    return "$PROJ_DIR$/../../../" + filename


def create_file(data, filename):
    """ Create *_opts files """
    with open(filename, "w") as f:
        f.write(data)


def get_element_value(element_dict, project_name):
    element_dict["OGChipSelectEditMenu"] = project_cfg.iar_ogcmenu.replace(
        ' ', '\t', 1)
    element_dict["IlinkOutputFile"] = project_name

    # patten = re.compile(r".*--config\s+(.*\.icf).*")
    # match = patten.match(project_cfg.global_ldflags)
    # if match:
    #     global appdir
    #     if appdir == "":
    #         icffile = convert_file_name(match.group(1))
    #     else:
    #         aos_sdk = os.environ.get("AOS_SDK_PATH")
    #         icffile = os.path.join(aos_sdk, match.group(1))
    element_dict["IlinkIcfFile"] = "settings/" + project_name + '.icf'


def add_sub_group(group, file_name):
    __group = group
    #确定是否有 sub_group
    file_name = file_name.replace("\\", '/')
    tmp_list = file_name.split("/")
    tmp_list = tmp_list[2:-1]
    if (tmp_list == []):  #无 sub_group , 直接返回
        return __group

    for name in tmp_list:  # 遍历所有子文件夹,检查sub_group是否存在
        need_creat = True
        for tmp_group in __group.findall('group'):
            if tmp_group.find('name').text == name:
                need_creat = False
                __group = tmp_group
                break
        if (need_creat):
            __group = SubElement(__group, 'group')
            group_name = SubElement(__group, 'name')
            group_name.text = name

    return __group  #添加sub_group完成


def add_group(parent, name, files):
    group = SubElement(parent, 'group')  #添加group
    group_name = SubElement(group, 'name')
    group_name.text = name

    for f in files:  #添加源文件到group
        sub_group = add_sub_group(group, f)
        file = SubElement(sub_group, 'file')
        file_name = SubElement(file, 'name')
        # if repeat_path.count(f):#存在重复文件
        #     dirname = os.path.dirname(f)
        #     includes.append(dirname)
        #     fnewName = os.path.basename(dirname) + '_' + os.path.basename(f)
        #     #fnewPath = project_path+'/'+fnewName
        #     #print 'copy', f, 'to', fnewPath
        #     #shutil.copyfile(f,fnewPath)
        #     f = "$PROJ_DIR$/"+fnewName
        #     file_name.text = f
        # else:
        file_name.text = convert_file_name(f)
    return


'''
    group_config = SubElement(group, 'configuration')
    group_config_name = SubElement(group_config, 'name')
    group_config_name.text = 'Debug'

    group_config_settings = SubElement(group_config, 'settings')
    group_settings_name = SubElement(group_config_settings, 'name')
    group_settings_name.text = 'ICCARM'

    group_settings_data = SubElement(group_config_settings, 'data')
    group_data_option = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option, 'name')
    group_option_name.text = 'IExtraOptionsCheck'
    group_option_state = SubElement(group_data_option, 'state')
    group_option_state.text = '1'

    group_data_option2 = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option2, 'name')
    group_option_name.text = 'IExtraOptions'
    group_option_state = SubElement(group_data_option2, 'state')
    group_option_state.text = '-f ' + OPT_DIR + name + ".c_opts"

    group_data_option3 = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option3, 'name')
    group_option_name.text = 'CCIncludePath2'
    for path in includes:
        include_path = SubElement(group_data_option3, 'state')
        include_path.text = "$PROJ_DIR$/../../" + path

    # for i in includes:#存在重复文件时的处理(暂未使用)
    #     i = i.replace("-I","")
    #     stateTemp = SubElement(group_data_option3, 'state')
    #     stateTemp.text = convert_file_name(i)

    group_config_settings2 = SubElement(group_config, 'settings')
    group_settings_name = SubElement(group_config_settings2, 'name')
    group_settings_name.text = 'AARM'

    group_settings_data = SubElement(group_config_settings2, 'data')
    group_data_option = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option, 'name')
    group_option_name.text = 'AExtraOptionsCheckV2'
    group_option_state = SubElement(group_data_option, 'state')
    group_option_state.text = '0'

    group_data_option2 = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option2, 'name')
    group_option_name.text = 'AExtraOptionsV2'

    group_option_state = SubElement(group_data_option2, 'state')
    group_option_state.text = '-f ' + OPT_DIR + name + ".as_opts"

    group_data_option3 = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option3, 'name')
    group_option_name.text = 'AUserIncludes'
    for i in includes:  #添加头文件
        i = i.replace("-I", "")
        stateTemp = SubElement(group_data_option3, 'state')
        stateTemp.text = convert_file_name(i)
    '''

def gen_icf(prj_path, prj_name, icf_data):
    icf_path = "%s\\settings\\%s.icf" %(prj_path, prj_name)
    data_list = []
    with open("template/project/template.icf", "r", encoding='utf-8') as fd :
        for data in fd:
            for key, val in icf_data.items():
                pos = data.find(key)
                if (pos < 0):
                    continue
                data = data[:pos]
                data += "%s = %s;\r\n" %(key, val)
                break
            data_list.append(data)

    with open(icf_path, "w", encoding="utf-8") as fd :
        for data in data_list:
            fd.write(data)
    return

# automation to do
def changeItemForMcu(tree, element_dict, project_name):
    for config in tree.findall('configuration'):
        for settings in config.findall('settings'):
            if settings.find('name').text == 'ILINK':
                data = settings.find('data')
                for option in data.findall('option'):
                    if option.find('name').text == 'IlinkOutputFile':
                        option.find('state').text = project_name + '.out'
                    #if option.find('name').text == 'IlinkIcfFile':
                    #    option.find(
                    #        'state').text = element_dict["IlinkIcfFile"]
            if settings.find('name').text == 'OBJCOPY':
                data = settings.find('data')
                for option in data.findall('option'):
                    if option.find('name').text == 'OOCOutputFile':
                        option.find('state').text = project_name + '.bin'

            if settings.find('name').text == 'General':
                data = settings.find('data')
                for option in data.findall('option'):
                    if option.find('name').text == 'OGChipSelectEditMenu':
                        option.find('state').text = element_dict[
                            "OGChipSelectEditMenu"]
                    if option.find('name').text == 'GFPUDeviceSlave':
                        option.find('state').text = element_dict[
                            "OGChipSelectEditMenu"]


work_space_content = '''<?xml version="1.0" encoding="UTF-8"?>

<workspace>
  <project>
    <path>$WS_DIR$\%s\%s</path>
  </project>
  <project>
    <path>$WS_DIR$\%s\%s</path>
  </project>
  <batchBuild/>
</workspace>


'''


def gen_workspace(target, prj_list):
    # make an workspace
    workspace = target.replace('.ewp', '.eww')
    xml = work_space_content % (prj_list[0], prj_list[0] + '.ewp', prj_list[1],
                                prj_list[1] + '.ewp')
    with open(workspace, "w") as out:
        out.write(xml)

    ewdfilename = 'template/project/template.ewd'
    for prj in prj_list:
        ewdfilename_new = "/".join(
            target.split("/")[0:-1])
        ewdfilename_new = ewdfilename_new +"/%s/%s.ewd" %(prj, prj)
        if os.path.isfile(ewdfilename):
            shutil.copyfile(ewdfilename, ewdfilename_new)
    return


def set_prj_param(root, script, project_path, project_name):
    includes = []
    defines = []
    icf_data = {}

    for group in script:
        if (group["name"] == "comm-setting"):
            includes = group["include"]
            defines = group["define"]
            icf_data.update(group["icf"])
            script.remove(group)  #移除 "comm-setting"项
            break

    for tag in root.findall('configuration/settings/data/option'):
        if (tag.find("name").text == "CCIncludePath2"):
            tag.remove(tag.find("state"))
            for include in includes:
                path = SubElement(tag, 'state')
                path.text = "$PROJ_DIR$\\..\\..\\..\\" + include
            continue
        if (tag.find("name").text == "CCDefines"):
            tag.remove(tag.find("state"))
            for define in defines:
                path = SubElement(tag, 'state')
                path.text = define
            continue
        if (tag.find("name").text == "IExtraOptionsCheck"):
            tag.remove(tag.find("state"))
            path = SubElement(tag, 'state')
            path.text = '1'
            continue
        if (tag.find("name").text == "OutputFile") or \
            (tag.find("name").text == "AOutputFile"):
            tag.remove(tag.find("state"))
            path = SubElement(tag, 'state')
            path.text = '$FILE_BNAME$.o'
            continue
        if (tag.find("name").text == "IlinkProgramEntryLabel"):
            tag.remove(tag.find("state"))
            path = SubElement(tag, 'state')
            path.text = '__iar_program_start'
            continue
        if (tag.find("name").text == "IlinkIcfOverride"):
            tag.remove(tag.find("state"))
            for include in includes:
                path = SubElement(tag, 'state')
                path.text = "1"
            continue
        if (tag.find("name").text == "IlinkIcfFile"):
            tag.remove(tag.find("state"))
            path = SubElement(tag, "state")
            path.text = '$PROJ_DIR$\\settings\\%s.icf' %(project_name)
            continue
    gen_icf(project_path, project_name, icf_data)
    return


repeat_path = []


# target:输出文件路径
# script:从project_cfg.py中获取到的参数
# project_name: 工程名(firmware or boot)
def gen_project(target, script, project_name):
    project_path = os.path.dirname(os.path.abspath(target))

    project_opts_path = os.path.join(project_path, "opts")
    project_cfg_path = os.path.join(project_path, "settings")
    if not os.path.isdir(project_opts_path):
        os.makedirs(project_opts_path)  #创建 "opts" 文件夹
    if not os.path.isdir(project_cfg_path):
        os.makedirs(project_cfg_path)  #创建 "settings" 文件夹

    get_element_value(element_dict, project_name)  #获取参数

    # boardname = project_name.split("@")[1]
    # projfilename = 'build/scripts/template_%s.ewp' % (boardname)
    # if os.path.exists(projfilename) == False:
    #     projfilename = 'build/scripts/template.ewp'
    projfilename = 'template/project/template.ewp'
    tree = etree.parse(projfilename)
    root = tree.getroot()

    set_prj_param(root, script, project_path, project_name)
    existedFileNameString = []
    # copy repeat source file and replace old one
    for group in script:
        for filePath in group['src']:
            filename = os.path.splitext(basename(filePath))
            if existedFileNameString.count(filename):
                repeat_path.append(filePath)  #重复文件
            else:
                existedFileNameString.append(filename)
        '''if group['c_opts_iar']:
            filename = os.path.join(project_opts_path, "%s.c_opts" % group['name'])
            group['c_opts_iar'] = group['c_opts_iar'].replace(' -I', ' -I'+os.path.abspath('.').replace("\\", "/")+"/../../")
            create_file(group['c_opts_iar'], filename)

        if group['as_opts_iar']:
            filename = os.path.join(project_opts_path, "%s.as_opts" % group['name'])
            # strip '--cpu xxx' from as_opts_iar which causes IAR IDE build failed
            as_opts_tmp = re.sub(r'^\s*Cortex-M\d+ ','',group['as_opts_iar'])
            create_file(as_opts_tmp, filename)'''

    if len(repeat_path):
        print('repeat name files:', repeat_path)
        print('will copy them to ' + project_path + '/ !')

    # add group
    for group in script:
        add_group(root, group['name'], group['src'])

    changeItemForMcu(tree, element_dict, project_name)
    gen_indent(root)
    projString = etree.tostring(root, encoding='utf-8')
    with open(target, "w") as out:  #写入工程文件
        out.write(projString.decode())
    return


def main():
    prj_list = ["boot", "firmware"]
    print('Making iar workspace: ' + workspace)
    for project in prj_list:
        projectPath = "../iar/%s/%s.ewp" % (project, project)
        gen_project(projectPath, Projects[project], project)
    projectPath = "../iar/%s.ewp" % ("nrf24l01")
    gen_workspace(projectPath, prj_list)

    print('iar project: ' + projectPath + ' has generated over')


if __name__ == "__main__":
    main()
