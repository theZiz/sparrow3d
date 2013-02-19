#!/usr/bin/python

# toolbar.py

import wx

class MyToolBar(wx.Frame):
    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, id, title, wx.DefaultPosition, wx.Size(640, 480))

        toolbarPanel = wx.Panel(self, -1)
        toolbar = wx.BoxSizer(wx.HORIZONTAL)
        toolbar.Add(wx.BitmapButton(toolbarPanel, 1,  wx.Image('projectManagerFiles/new.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()),0)
        toolbar.Add(wx.BitmapButton(toolbarPanel, 2,  wx.Image('projectManagerFiles/open.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()),0)
        toolbar.Add(wx.BitmapButton(toolbarPanel, 3,  wx.Image('projectManagerFiles/save.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()),0)
        toolbar.Add(wx.BitmapButton(toolbarPanel, 4,  wx.Image('projectManagerFiles/save-as.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()),0)
        
        toolbar.Add(wx.StaticLine(toolbarPanel, -1, size = (5,44),style = wx.LI_VERTICAL),0)

        toolbar.Add(wx.BitmapButton(toolbarPanel, 5,  wx.Image('projectManagerFiles/make.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()),0)
        toolbar.Add(wx.BitmapButton(toolbarPanel, 6,  wx.Image('projectManagerFiles/clear.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()),0)
        toolbar.Add(wx.BitmapButton(toolbarPanel, 7,  wx.Image('projectManagerFiles/run.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()),0)
        toolbar.Add(wx.BitmapButton(toolbarPanel, 8,  wx.Image('projectManagerFiles/install.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()),0)
        toolbar.Add(wx.StaticLine(toolbarPanel, -1, size = (5,44),style = wx.LI_VERTICAL),0)
        
        toolbar.Add(wx.BitmapButton(toolbarPanel, 9,  wx.Image('projectManagerFiles/exit.png', wx.BITMAP_TYPE_PNG).ConvertToBitmap()),0)
        toolbarPanel.SetSizer(toolbar)
        self.Centre()
        
        self.Bind(wx.EVT_BUTTON, self.OnExit, id=9)

    def OnExit(self, event):
        self.Close()
        
class MyApp(wx.App):
    def OnInit(self):
        frame = MyToolBar(None, -1, 'toolbar.py')
        frame.Show(True)
        return True

app = MyApp(0)
app.MainLoop()
