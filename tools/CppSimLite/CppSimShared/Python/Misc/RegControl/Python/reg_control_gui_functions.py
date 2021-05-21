try:
    # Python 2 uses the following:
    import Tkinter as tk
    import tkFont
    import ttk
except ImportError:
    # Python 3 uses the following:
    import tkinter as tk
    import tkinter.font as tkFont
    import tkinter.ttk as ttk

class MultiColumnListBox(object):
    """TreeView widget as multi-column listbox"""

    def __init__(self,header_name_list,data_2d_list,num_rows):
        self.num_rows = num_rows
        self.header_name_list = header_name_list
        self.data_2d_list = data_2d_list
        self.tree = None
        self.root = tk.Tk()
        self.root.title("Register Field Names and Values")
        self._setup_widgets()
        self._build_tree()
        self.root.mainloop()
        
    def _setup_widgets(self):
        container = ttk.Frame()
        style = ttk.Style()
        style.configure('Treeview', rowheight=25)
        container.pack(fill='both', expand=True)
        # create a treeview with dual scrollbars
        self.tree = ttk.Treeview(columns=self.header_name_list, show="headings",height=self.num_rows)
        vsb = ttk.Scrollbar(orient="vertical",
            command=self.tree.yview)
#        hsb = ttk.Scrollbar(orient="horizontal",
#            command=self.tree.xview)
#        self.tree.configure(yscrollcommand=vsb.set,
#            xscrollcommand=hsb.set)
        self.tree.configure(yscrollcommand=vsb.set) 
        self.tree.grid(column=0, row=0, sticky='nsew', in_=container)
        vsb.grid(column=1, row=0, sticky='ns', in_=container)
#        hsb.grid(column=0, row=1, sticky='ew', in_=container)
        container.grid_columnconfigure(0, weight=1)
        container.grid_rowconfigure(0, weight=1)

    def _build_tree(self):
        for col in self.header_name_list:
            self.tree.heading(col, text=col.title(),
                              command=lambda c=col: self.sortby(self.tree, c, 0),anchor=tk.W)
            # adjust the column's width to the header string
            self.tree.column(col,
                width=tkFont.Font().measure(col.title()))
        odd_or_even = 'evenrow'
        for item in self.data_2d_list:
            if odd_or_even == 'evenrow':
                odd_or_even = 'oddrow'
            else:
                odd_or_even = 'evenrow'
            self.tree.insert('', 'end', values=item, tags = (odd_or_even))
            # adjust column's width if necessary to fit each value
            for ix, val in enumerate(item):
                col_w = tkFont.Font().measure(val)
                if self.tree.column(self.header_name_list[ix],width=None)<col_w:
                    self.tree.column(self.header_name_list[ix], width=col_w)
        # other useful colors:   'white', white smoke', 'light gray'
        self.tree.tag_configure('oddrow', background='lemon chiffon')
        self.tree.tag_configure('evenrow', background='light cyan')

    def sortby(self,tree, col, descending):
        print("col = %s" % col)
        """sort tree contents when a column header is clicked on"""
        # grab values to sort
        data = [(tree.set(child, col), child) \
            for child in tree.get_children('')]
        # if the data to be sorted is numeric change to float
        #data =  change_numeric(data)
        # now sort the data in place
        data.sort(reverse=descending)
        for ix, item in enumerate(data):
            tree.move(item[1], '', ix)
        # switch the heading so it will sort in the opposite direction
        tree.heading(col, command=lambda col=col: self.sortby(tree, col, int(not descending)))
