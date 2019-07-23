# -*- coding: utf-8 -*-
# @Time    : 2019/7/23 15:05
# @Author  : lzhn5907

import sys
import os
from common import *

NO_FILTER             = 'no_filter'
ALPHA_TEST_FILTER       = 'alpha_filter'
NORMAL_FILTER           = 'normal_filter'

FILTER_CMD_ARGV = 0
FILTER_POSTFIX = 1
AlphaToCoverageFilter   = 'alphaToCoverage'
ErrorDiffusionFilter    = 'errorDiffusion'
AlphaPyramidFilter      = 'alphaPyramid'

alpha_test_filter_map = {
    NO_FILTER: {
        FILTER_CMD_ARGV:    "None",
        FILTER_POSTFIX:     "_none"
    },
    AlphaToCoverageFilter:{
        FILTER_CMD_ARGV:    "AlphaToCoverage",
        FILTER_POSTFIX:     "_alphaToCoverage"
    },
    ErrorDiffusionFilter:{
        FILTER_CMD_ARGV:    "ErrorDiffusion" ,
        FILTER_POSTFIX:     "_errorDiffusion"
    },
    AlphaPyramidFilter:{
        FILTER_CMD_ARGV:    "AlphaPyramid",
        FILTER_POSTFIX:     "_alphaPyramid"
    }
}

if __name__ == "__main__":
    idx = 1
    #input_file_name = r"F:\1_proj_dir\workspace_prgramer\client\res\testres\birch_a_summer_tree_copy\texture\t_birch_leaves_b_d.tga"
    input_file_name = r"F:\1_proj_dir\workspace_prgramer\client\res\testres\birch_a_summer_tree_copy\texture\t_birch_leaves_d.tga"
    output_file_name = ""
    filter_type = NO_FILTER

    '''
    read input data
    '''
    while len(sys.argv) > idx:
        #if sys.argv[idx] == "-type":
        input_file_name = sys.argv[idx]
        idx += 1

    '''
    start do filter
    '''
    for v in alpha_test_filter_map.values():
        filter_type_cmd_arg = v[FILTER_CMD_ARGV]
        filter_postfix = v[FILTER_POSTFIX]
        pre, ext = os.path.splitext(input_file_name)
        output_file_name = pre + filter_postfix + ".dds"
        cmd_str = filter_tool_path + r" -alpha -rgb -alphaRef 127 -alg " + \
                  filter_type_cmd_arg + " " + input_file_name + " "  + output_file_name
        os.system(cmd_str)
        print "do alpha_test_filter with algType: "+ filter_type_cmd_arg + " output: " + output_file_name

