using System;
using System.Collections;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Xml.Linq;

using MySql.Data.MySqlClient;

public partial class Chart : System.Web.UI.Page
{
    private MySQLAccess mySQLAccess = new MySQLAccess();
    private DateTime zeroDateTime;
    private String zeroDateTimeString;
    private DateTime offsetDateTime;
    private String offsetDateTimeString;

    private void getUrlParameters()
    {
        String archiveDatabaseName = null;
        String mainDatabaseName = null;

        if ((mainDatabaseName = Request.QueryString["mainDatabaseName"]) == null)
        {
            mySQLAccess.setMainDatabaseName("main");
        }
        else
        {
            mySQLAccess.setMainDatabaseName(mainDatabaseName);
        }

        try
        {
            if ((archiveDatabaseName = Request.QueryString["archiveDatabaseName"]) == null)
            {
                mySQLAccess.setArchiveDatabaseName();
            }
            else
            {
                mySQLAccess.setArchiveDatabaseName(archiveDatabaseName);
            }
        }
        catch (Exception ex)
        {
            Message.setErrorMessage("accessing main database");
            Message.setDetailedMessage(ex.Message);
            Response.Redirect("error.aspx", true);
        }
    }

    private bool dataPresent(PlotData p1,
                             PlotData p2,
                             PlotData p3,
                             PlotData p4)
    {
        bool dataPresent = false;

        if ((p1 != null) || (p2 != null) || (p3 != null) || (p4 != null))
        {
            dataPresent = true;
        }

        return dataPresent;
    }

    private PlotData getData(String tableName, String fieldName, String startTime, String endTime)
    {
        PlotData plotData;

        mySQLAccess.setDataTableName(tableName);
        mySQLAccess.setDataFieldName(fieldName);
        plotData = mySQLAccess.getData(startTime, endTime);
        if (plotData != null)
        {
            plotData.setHoursFromZero(zeroDateTime);
        }

        return plotData;
    }

    protected void Page_Load(object sender, EventArgs e)
    {
        PlotData historical_scada, recent_scada, last_forecast, merged_forecast;
        Plotter plotter;

        getUrlParameters();

        try
        {
            mySQLAccess.setLastForecastTableName();
        }
        catch (Exception ex)
        {
            Message.setErrorMessage("accessing archive database");
            Message.setDetailedMessage(ex.Message);
            Response.Redirect("error.aspx", true);
        }
        setZeroDateTime();

        mySQLAccess.setWindFarmId("4");

        historical_scada = getData("iScadaF2_" + zeroDateTimeString, "WF_Power", zeroDateTime.AddHours(-48.0).ToString("yyyy-MM-dd HH:mm"), zeroDateTime.ToString("yyyy-MM-dd HH:mm"));
        merged_forecast = getData("iWindF3F2_" + zeroDateTimeString, "WF_Power", zeroDateTime.ToString("yyyy-MM-dd HH:mm"), zeroDateTime.AddHours(48.0).ToString("yyyy-MM-dd HH:mm"));
        recent_scada = getData("iScadaF1_" + zeroDateTimeString, "WF_Power", zeroDateTime.AddHours(0.1).ToString("yyyy-MM-dd HH:mm"), zeroDateTime.AddHours(6.0).ToString("yyyy-MM-dd HH:mm"));
        setOffsetDateTimeString(-6.0);
        last_forecast = getData("iWindF2_" + offsetDateTimeString, "WF_Power", offsetDateTime.ToString("yyyy-MM-dd HH:mm"), offsetDateTime.AddHours(48.0).ToString("yyyy-MM-dd HH:mm"));

        /*mySQLAccess.setDataTableName("iWindF3F2_" + zeroDateTimeString);
        mySQLAccess.setDataFieldName("WF_Power");
        merged_forecast = mySQLAccess.getData(zeroDateTime.ToString("yyyy-MM-dd HH:mm"), zeroDateTime.AddHours(48.0).ToString("yyyy-MM-dd HH:mm"));
        if (merged_forecast != null)
        {
            merged_forecast.setHoursFromZero(zeroDateTime);
        }

        mySQLAccess.setDataTableName("iScadaF1_" + zeroDateTimeString);
        mySQLAccess.setDataFieldName("WF_Power");
        recent_scada = mySQLAccess.getData(zeroDateTime.AddHours(0.1).ToString("yyyy-MM-dd HH:mm"), zeroDateTime.AddHours(6.0).ToString("yyyy-MM-dd HH:mm"));
        if (recent_scada != null)
        {
            recent_scada.setHoursFromZero(zeroDateTime);
        }

        setOffsetDateTimeString(-6.0);
        mySQLAccess.setDataTableName("iWindF2_" + offsetDateTimeString);
        mySQLAccess.setDataFieldName("WF_Power");
        last_forecast = mySQLAccess.getData(offsetDateTime.ToString("yyyy-MM-dd HH:mm"), offsetDateTime.AddHours(48.0).ToString("yyyy-MM-dd HH:mm"));
        if (last_forecast != null)
        {
            last_forecast.setHoursFromZero(zeroDateTime);
        }*/

        plotter = new ChartDirectorPlotter(WebChartViewer1);
        if (dataPresent(historical_scada, recent_scada, last_forecast, merged_forecast))
        {
            plotter.plotData(recent_scada, historical_scada, last_forecast, merged_forecast, "Hours from last prediction (" + zeroDateTime.ToString("dd/MM/yyyy HH:mm") + ")");
        }
        else
        {
            Message.setErrorMessage("no data to plot");
            Message.setDetailedMessage("No data in database " + mySQLAccess.getArchiveDatabaseName());
            Response.Redirect("error.aspx", true);
        }
    }

    private void setZeroDateTime()
    {
        int start;
        int year, month, day, hour;
        zeroDateTimeString = mySQLAccess.getLastForecastTableName();
        start = zeroDateTimeString.LastIndexOf('_') + 1;
        zeroDateTimeString = zeroDateTimeString.Substring(start, zeroDateTimeString.Length - start);

        year = Int32.Parse(zeroDateTimeString.Substring(0, 4));
        month = Int32.Parse(zeroDateTimeString.Substring(4, 2));
        day = Int32.Parse(zeroDateTimeString.Substring(6, 2));
        hour = Int32.Parse(zeroDateTimeString.Substring(8, 2));
        zeroDateTime = new DateTime(year, month, day, hour, 0, 0);
    }

    private void setOffsetDateTimeString(double offsetHours)
    {
        offsetDateTime = zeroDateTime.AddHours(offsetHours);
        offsetDateTimeString = offsetDateTime.ToString("yyyyMMddHH");
    }

    protected double[] simulateHoursFromZero(double[] scada, double start)
    {
        int length = scada.Length;
        double[] xData = new double[length];
        double value = start;

        for (int i = 0; i < length; i++)
        {
            xData[i] = value;
            value += 1.0;
        }

        return xData;
    }
}
