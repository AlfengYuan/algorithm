# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import xlrd  ## xlrd==1.2.0
import csv

def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    xlsxPath = "231115.xlsx"
    # 打开excel表格
    data_excel = xlrd.open_workbook(xlsxPath)

    # 获取所有sheet名称
    names = data_excel.sheet_names()

    # 获取book中的sheet工作表的三种方法,返回一个xlrd.sheet.Sheet()对象
    table = data_excel.sheets()[0]  # 通过索引顺序获取sheet
    # table = data_excel.sheet_by_index(sheetx=0)  # 通过索引顺序获取sheet
    total_table = data_excel.sheet_by_name(sheet_name='总表')  # 通过名称获取
    xugong_table = data_excel.sheet_by_name(sheet_name='徐工盒子')
    my_table = data_excel.sheet_by_name(sheet_name='我司盒子')

    xugong_code = [i.value for i in xugong_table.col(colx=1)[1:]]
    my_code = [i.value for i in my_table.col(colx=2)[1:]]
    my_mdmc = [i.value for i in my_table.col(colx=0)[1:]]
    my_zcbh = [i.value for i in my_table.col(colx=1)[1:]]
    my_pp = [i.value for i in my_table.col(colx=7)[1:]]
    my_lx = [i.value for i in my_table.col(colx=8)[1:]]
    my_xhjc = [i.value.split("(")[0] for i in my_table.col(colx=5)[1:]]
    my_xhqc = [i.value for i in my_table.col(colx=5)[1:]]

    xg_ccbh = [i.value for i in xugong_table.col(colx=2)[1:]]
    my_info = [i.value for i in my_table.col(colx=3)[1:]]
    my_chuzu = [i.value for i in my_table.col(colx=6)[1:]]

    with open(r'./test2.csv', mode='w', newline='', encoding='utf8') as cf:
        wf = csv.writer(cf)
        title = ["序号", "门店名称", "基地", "资产编号", "品牌", "规格", "型号简称", "出厂编号", "徐工盒子信息",
                 "我司盒子信息", "是否两个盒子", "租约信息", "最新状态", "动力源", "类型", "门店保留/不保留",
                 "华夏资产号", "出厂日期", "型号", "验收日期", "转移日期"]

        wf.writerow(title)

        for i in range(len(xugong_code)):
            if xugong_code[i] in my_code:
                my_index = my_code.index(xugong_code[i])
                ccrq = xugong_code[i].split('-')[2] + xugong_code[i].split('-')[3]
                data = [i, my_mdmc[my_index], my_mdmc[my_index], my_zcbh[my_index], my_pp[my_index],
                        my_lx[my_index], my_xhjc[my_index], xg_ccbh[i], xg_ccbh[i], my_info[my_index], "否",
                        my_chuzu[my_index], "冻结", "蓄电池", my_lx[my_index], "保留门店", xugong_code[i], ccrq,
                        my_xhqc[my_index], "无相关信息", "无相关信息"
                        ]
                wf.writerow(data)


    # # excel工作表的行列操作
    # n_rows = table.nrows  # 获取该sheet中的有效行数
    # n_cols = table.ncols  # 获取该sheet中的有效列数
    # # row_list = table.row(rowx=0)  # 返回某行中所有的单元格对象组成的列表
    # cols_list_0 = table.col(colx=0)  # 返回某列中所有的单元格对象组成的列表
    # cols_list_1 = table.col(colx=1)
    # # print(cols_list_0)
    # # print(cols_list_1)
    # assert len(cols_list_0) == len(cols_list_1)
    # label_names = dict()
    # for i in range(1, len(cols_list_0)):
    #     label_names[cols_list_0[i].value] = cols_list_1[i].value

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
