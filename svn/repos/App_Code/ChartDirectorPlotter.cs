using System;
using System.Data;
using System.Configuration;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Xml.Linq;

using ChartDirector;

public class ChartDirectorPlotter : Plotter
{
    XYChart chart;
    WebChartViewer webChartViewer;

	public ChartDirectorPlotter(WebChartViewer webChartViewer)
	{
        this.webChartViewer = webChartViewer;
	}

    protected void initialisePlot(String title)
    {
        chart.setDropShadow();

        // Add a title using 18 pts Times New Roman Bold Italic font. #Set top margin to
        // 16 pixels.
        chart.addTitle("Wind Farm Power Data", "Times New Roman Bold Italic", 18
            ).setMargin2(0, 0, 16, 0);

        // Set the plotarea at (60, 80) and of 810 x 375 pixels in size. Use transparent
        // border and dark grey (444444) dotted grid lines
        /*PlotArea plotArea = chart.setPlotArea(60, 80, 810, 375, -1, -1, Chart.Transparent,
            chart.dashLineColor(0x444444, 0x000101), -1);*/
        PlotArea plotArea = chart.setPlotArea(60, 80, 710, 375, -1, -1, Chart.Transparent,
            chart.dashLineColor(0x444444, 0x000101), -1);
        // Add a legend box where the top-center is anchored to the horizontal center of
        // the plot area at y = 45. Use horizontal layout and 10 points Arial Bold font,
        // and transparent background and border.
        LegendBox legendBox = chart.addLegend(plotArea.getLeftX() + plotArea.getWidth() / 2,
            45, false, "Arial Bold", 10);
        legendBox.setAlignment(Chart.TopCenter);
        legendBox.setBackground(Chart.Transparent, Chart.Transparent);

        // Set x-axis tick density to 75 pixels and y-axis tick density to 30 pixels.
        // ChartDirector auto-scaling will use this as the guidelines when putting ticks
        // on the x-axis and y-axis.
        chart.yAxis().setTickDensity(30);
        chart.xAxis().setTickDensity(75);

        // Set all axes to transparent
        chart.xAxis().setColors(Chart.Transparent);
        chart.yAxis().setColors(Chart.Transparent);

        // Set the x-axis margins to 15 pixels, so that the horizontal grid lines can
        // extend beyond the leftmost and rightmost vertical grid lines
        chart.xAxis().setMargin(15, 15);

        // Set axis label style to 8pts Arial Bold
        chart.xAxis().setLabelStyle("Arial Bold", 8);
        chart.yAxis().setLabelStyle("Arial Bold", 8);
        chart.yAxis2().setLabelStyle("Arial Bold", 8);

        // Add axis title using 10pts Arial Bold Italic font
        chart.xAxis().setTitle(title, "Arial Bold Italic", 10);

        // Add axis title using 10pts Arial Bold Italic font
        chart.yAxis().setTitle("Power (MWatts)", "Arial Bold Italic", 10);

        //include tool tip for the chart
        /*WebChartViewer1.ImageMap = chart.getHTMLImageMap("", "",
            "title=Plot");*/
    }

    protected void addLine(PlotData plotData, String title, Int32 colour)
    {
        LineLayer layer;

        if (plotData == null)
        {
            layer = chart.addLineLayer2();
            layer.addDataSet(null, colour, title + ": NO DATA").setDataSymbol(
                Chart.GlassSphere2Shape, 6);
            //layer.setXData(plotData.getHoursFromZero());
            layer.setLineWidth(3);
        }
        else
        {
            layer = chart.addLineLayer2();
            layer.addDataSet(plotData.getData(), colour, title).setDataSymbol(
                Chart.GlassSphere2Shape, 6);
            layer.setXData(plotData.getHoursFromZero());
            layer.setLineWidth(3);
        }
    }

    public void plotData(PlotData recent_scada, PlotData historical_scada, PlotData last_forecast, PlotData merged_forecast, String title)
    {
        //chart = new XYChart(900, 500);
        chart = new XYChart(800, 500);
        initialisePlot(title);

        // Add data lines to chart
        addLine(recent_scada, "Live SCADA", 0x00ff00);
        addLine(historical_scada, "Past SCADA", 0xff0000);
        addLine(last_forecast, "Last forecast", 0xffff00);
        addLine(merged_forecast, "Merged_forecast", 0xff00ff);

        // Output the chart
        webChartViewer.Image = chart.makeWebImage(Chart.PNG);
    }
}