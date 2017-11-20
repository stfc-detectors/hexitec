////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file GigE.h
//! \brief Header file - Hexitec GigE DLL
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef GIGE_EXPORTS
#define GIGE_API __declspec(dllexport)
#else
#define GIGE_API __declspec(dllimport)
#endif

#include <windows.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	A macro that defines the hexitec setup register size. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
#define AS_HEXITEC_SETUP_REGISTER_SIZE 10

///////////////////////////////////////////////////////////////////////////////////////////////////
// <summary>
// function typedef for CallBack function to signalize a filled transfer buffer to a third party
// application.
// </summary>
//
// <remarks>	Frank, 30.07.2015. </remarks>
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef void (__cdecl *p_bufferCallBack)(
	PUCHAR						transferBuffer,								// pointer to current transfer buffer
	ULONG						frameCount );								// number of valid frames within the transfer buffer

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing the uchar. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum HexitecGain : UCHAR {
	/// <summary>	An enum constant representing the Hexitec gain high option. </summary>
	AS_HEXITEC_GAIN_HIGH				= 0,
	/// <summary>	An enum constant representing the Hexitec gain low option. </summary>
	AS_HEXITEC_GAIN_LOW					= 1
	} HexitecGain;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing the uchar. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum HexitecAdcSample : UCHAR {
	/// <summary>	An enum constant representing the ADC sample at rising edge option. </summary>
	AS_HEXITEC_ADC_SAMPLE_RISING_EDGE	= 0,
	/// <summary>	An enum constant representing the ADC sample at falling edge option. </summary>
	AS_HEXITEC_ADC_SAMPLE_FALLING_EDGE	= 1
	} HexitecAdcSample;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing the uchar. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum Control : UCHAR {
	/// <summary>	An enum constant representing the control disabled option. </summary>
	AS_CONTROL_DISABLED					= 0,
	/// <summary>	An enum constant representing the control enabled option. </summary>
	AS_CONTROL_ENABLED					= 1
	} Control;

#ifndef _M_X64
#pragma pack(push)
#pragma pack(1)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing the GigE device information strings. </summary>
/// 
/// <remarks>
/// The function GetDeviceInformation() sets the pointers within this structure. All pointer are
/// pointing to NULL terminated C-Strings which are valid as long as the GigE device instance is
/// valid.
/// </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct GigEDeviceInfoStr {

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the vendor string. </summary>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CONST LPSTR					Vendor;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the model string. </summary>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CONST LPSTR					Model;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the manufacturer information string. </summary>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CONST LPSTR					ManufacturerInfo;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the serial number string. </summary>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CONST LPSTR					SerialNumber;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the user definable identifier string. </summary>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CONST LPSTR					UserId;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the MAC address string. </summary>
	///
	/// <remarks>
	/// This string can be used as parameter 'deviceDescriptor' to open the specific device if calling
	/// InitDevice().
	/// </remarks>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CONST LPSTR					MacAddress;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the IP address string. </summary>
	///
	/// <remarks>
	/// This string can be used as parameter 'deviceDescriptor' to open the specific device if calling
	/// InitDevice(). The address may change after a power cycle.
	/// </remarks>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CONST LPSTR					IpAddress;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the net mask string. </summary>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CONST LPSTR					NetMask;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the gate way string. </summary>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CONST LPSTR					GateWay;
	} GigEDeviceInfoStr;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing a pointer to GigEDeviceInfoStr. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef GigEDeviceInfoStr	*GigEDeviceInfoStrPtr;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing a handle to GigEDeviceInfoStr. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef GigEDeviceInfoStr	**GigEDeviceInfoStrHdl;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing the GigE device system parameters. </summary>
///
/// <remarks>
/// These parameters are typically constant and may change only after hardware or firmware
/// changes.
/// </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct HexitecSystemConfig {
		
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Adjust the sample point. </summary>
	///
	/// <remarks>
	/// Adjust the sample point (time) when the signal is sampled by the ADC in relation to the sensor
	/// stimuli. Unsharpen edges and crosstalk between column can be minimized using this parameter.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | 2
	/// range    | 0..31
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	UCHAR						AdcDelay;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Adjust the position of the sync signals. </summary>
	///
	/// <remarks>
	/// Adjust the timing relationship between the sync signals and the pixel data. Column wise pixel
	/// shift can be adjusted using this parameter.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | 10
	/// range    | 0..?? todo
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	UCHAR						SyncSignalDelay;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets the sample edge of the ADC. </summary>
	///
	/// <remarks>
	/// Sets at which edge (rising / falling) of the ADC clock the ADC samples the signal.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_HEXITEC_ADC_SAMPLE_FALLING_EDGE
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	HexitecAdcSample			AdcSample;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Adjust the ADC clock phase. </summary>
	///
	/// <remarks>
	/// Adjust the phase between the ADC clock and the data stream of the ADC. This parameter can be
	/// used to avoid flickering pixels or false pixel values.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | 4
	/// range    | 0..11
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	UCHAR						AdcClockPhase;
	} HexitecSystemConfig;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing a pointer to HexitecSystemConfig. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef HexitecSystemConfig	*HexitecSystemConfigPtr;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing a handle to HexitecSystemConfig. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef HexitecSystemConfig	**HexitecSystemConfigHdl;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing the Hexitec sensor setup register. </summary>
///
/// <remarks>
/// The Hexitec sensor has 6 80bit setup register to configure the behaviour of the columns and
/// rows. 3 to configure the columns and 3 to configure the rows. This structure holds the values
/// within 3 times 10 bytes either to configure the columns or the rows.
/// </remarks>
///
/// <value>
/// 
/// byte	 | bit    | column / row
/// ---------|--------|--------
/// 0        | 0      | 0
/// 0        | 1      | 1
/// ...      | ...    | ...
/// 0        | 7      | 7
/// 1        | 0      | 8    
/// ...      | ...    | ...  
/// 9        | 7      | 79
/// </value>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct HexitecSetupRegister {
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets either the column / row is read out or skipped. </summary>
	///
	/// <remarks>
	/// Setting a bit to 1 enables reading out the corresponding column / row. Setting a bit to 0 the
	/// corresponding column / row is skipped. This means also that the resolution will change,
	/// changing this parameters. In column direction the sensor is divided into 4 parts which are read
	/// out simultaneously. The resolution change will not be 1 to 1 as it is for rows. It is 4 times
	/// the biggest part. E.g. Disabling 1 column only will result in a resolution of 80 columns.
	/// Disabling 1 column within each part will result in a resolution of 76 columns.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | [0..9] 0xFF
	/// range    | [0..9] 0x00..0xFF
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////		
	UCHAR						ReadEn[AS_HEXITEC_SETUP_REGISTER_SIZE];

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Enables the power for a column / row. </summary>
	///
	/// <remarks>
	/// Setting a bit to 1 enables the power for the corresponding column / row.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | [0..9] 0xFF
	/// range    | [0..9] 0x00..0xFF
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	UCHAR						PowerEn[AS_HEXITEC_SETUP_REGISTER_SIZE];

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Enables the calibration circuit for a column / row. </summary>
	///
	/// <remarks>
	/// Setting a bit to 1 enables the calibration circuit for the corresponding column / row.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | [0..9] 0x00
	/// range    | [0..9] 0x00..0xFF
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	UCHAR						CalEn[AS_HEXITEC_SETUP_REGISTER_SIZE];
	} HexitecSetupRegister;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing a pointer to HexitecSetupRegister. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef HexitecSetupRegister	*HexitecSetupRegisterPtr;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing a handle to HexitecSetupRegister. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef HexitecSetupRegister	**HexitecSetupRegisterHdl;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Defines an alias representing the Hexitec sensor / state machine configuration.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct HexitecSensorConfig {

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets the gain of the preamplifier. </summary>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_HEXITEC_GAIN_HIGH
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////		
	HexitecGain					Gain;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets the duration between the row and the S1 pulse. </summary>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | 1
	/// range    | 1..10000
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////		
	USHORT						Row_S1;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets the duration between the S1 and the SPH pulse. </summary>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | 1
	/// range    | 1..40
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////		
	UCHAR						S1_Sph;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets the duration between the SPH and the S2 pulse. </summary>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | 6
	/// range    | 1..40
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////		
	UCHAR						Sph_S2;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets the VCAL pulse duration. </summary>
	/// 
	/// <remarks>
	/// If set to 0 the VCAL pulse is deactivated.
	/// </remarks>
	/// 
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | 0
	/// range    | 0..22000
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////		
	USHORT						Vcal2_Vcal1;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Reserved. </summary>
	///
	/// <remarks>
	/// Internal state machine parameter. Keep at default value.
	/// </remarks>
	///  
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | 1
	/// range    | 1 todo
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////		
	UCHAR						WaitClockCol;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Reserved. </summary>
	///
	/// <remarks>
	/// Internal state machine parameter. Keep at default value.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | 8
	/// range    | 8 todo
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////		
	UCHAR						WaitClockRow;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets the sensor column setup registers. </summary>
	////////////////////////////////////////////////////////////////////////////////////////////////////		
	HexitecSetupRegister		SetupCol;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets the sensor row setup registers. </summary>
	////////////////////////////////////////////////////////////////////////////////////////////////////		
	HexitecSetupRegister		SetupRow;
	} HexitecSensorConfig;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing a pointer to HexitecSensorConfig. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef HexitecSensorConfig	*HexitecSensorConfigPtr;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing a handle to HexitecSensorConfig. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef HexitecSensorConfig	**HexitecSensorConfigHdl;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Defines an alias representing the operation mode / data path configuration.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct HexitecOperationMode {

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Starts uploading the offset values. </summary>
	///
	/// <remarks>
	/// Internal use only. Keep at default value.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_DISABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						DcUploadDarkCorrectionValues;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Starts collecting the offset values. </summary>
	///
	/// <remarks>
	/// Internal use only. Keep at default value.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_DISABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						DcCollectDarkCorrectionValues;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Enables hardware offset correction. </summary>
	///
	/// <remarks>
	/// Internal use only. Keep at default value.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_DISABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						DcEnableDarkCorrectionCountingMode;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Enables hardware offset correction. </summary>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_ENABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						DcEnableDarkCorrectionSpectroscopicMode;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Enables sending the hardware offset correction values. </summary>
	/// 
	/// <remarks>
	/// If enabled the current active offset correction values will be send. Use the function
	/// AcquireFrames() to get the offset values as a frame.
	/// </remarks>
	/// 
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_DISABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						DcSendDarkCorrectionValues;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Disables the VCAL pulse. </summary>
	/// 
	/// <remarks>
	/// If enabled the VCAL pulse is disabled.
	/// </remarks>
	/// 
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_ENABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						DcDisableVcalPulse;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Enables test pattern within offset correction function block. </summary>
	///
	/// <remarks>
	/// Internal use only. Keep at default value.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_DISABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						DcTestMode;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Enables trigger counting mode. </summary>
	///
	/// <remarks>
	/// Internal use only. Keep at default value.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_DISABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						DcEnableTriggeredCountingMode;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Starts uploading the threshold values. </summary>
	///
	/// <remarks>
	/// Internal use only. Keep at default value.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_DISABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						EdUploadThresholdValues;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Disables counting mode function block. </summary>
	///
	/// <remarks>
	/// Internal use only. Keep at default value.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_DISABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						EdDisableCountingMode;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Enables test pattern within counting mode function block. </summary>
	///
	/// <remarks>
	/// Internal use only. Keep at default value.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | AS_CONTROL_DISABLED
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	Control						EdTestMode;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets the number of frames used for the counting mode. </summary>
	///
	/// <remarks>
	/// Internal use only. Keep at default value.
	/// </remarks>
	///
	/// <value>
	/// property | value
	/// ---------|--------
	/// default	 | 0
	/// range	 | 0..65535
	/// </value>
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	USHORT						EdCycles;
	} HexitecOperationMode;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing a pointer to HexitecOperationMode. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef HexitecOperationMode	*HexitecOperationModePtr;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Defines an alias representing a handle to HexitecOperationMode. </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef HexitecOperationMode	**HexitecOperationModeHdl;

#ifndef _M_X64
#pragma pack(pop)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Acquires frames from the detector. </summary>
///
/// <remarks>
///		Acquires a specific number or continuously frames from the detector. The acquired frames
///		will be copied into a transfer buffer. Once a transfer buffer is filles the registered
///		callback function (RegisterTransferBufferReadyCallBack()) is called to transmit the pointer
///		and the number of frames within this buffer to a third party application for data
///		processing. Once the processing of the data within this buffer is done the buffer has to
///		be returned by calling ReturnBuffer() function. If the specified frames are acuired,
///		StopAcquisition() is called or an error occurs the function will return. Requires an opened
///		acuisition stream and pipeline (OpenStream() and CreatePipeline()).
///	</remarks>
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="frameCount">
///		[in]	Number of frames to acquire.
///		value | behaviour
///		------|--------
///		0     | continuous until StopAcquisition() is called
/// 	>0    | returns after specified number of frames are acquired
/// </param>
/// <param name="framesAcquired">
///		[out]	Returns the number of the acquired frames.
/// </param>
/// <param name="frameTimeOut">
///		[in]	Frame time out (ms) to acquire the first frame. Should be set to 2.5 times the frame
///		time if not triggered or to the expected time of incoming trigger time if triggered. But at
///		least to 25ms.
/// </param>
/// 
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	AcquireFrames(
	HANDLE							deviceHdl,
	ULONG							frameCount,
	PULONGLONG						framesAcquired,
	ULONG							frameTimeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Checks the installed FPGA firmware version. </summary>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="customerId">
///		[in,out]	Set to 0x01 (required customer id). Will be overwritten with the installed
///		value (version).
/// </param>
/// <param name="projectId">
///		[in,out]	Set to 0x12 (required project id). Will be overwritten with the installed
///		value (version).
/// </param>
/// <param name="version">
///		[in,out]	Set to 0x01 (required version). Will be overwritten with the installed value
///		(version).
/// </param>
/// <param name="forceEqualVersion">
/// 	[in]	Selects to check the installed version has to be equal or greater or equal as
/// 	the required version to pass.
///		value | behaviour
///		------|--------
///		0     | greater or equal
/// 	>0    | equal
/// </param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
/// </param>
/// 
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	CheckFirmware(
	HANDLE							deviceHdl,
	PUCHAR							customerId,
	PUCHAR							projectId,
	PUCHAR							version,
	UCHAR							forceEqualVersion,
	ULONG							timeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Closes a frame acquisition pipeline. </summary>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	ClosePipeline(
	HANDLE							deviceHdl );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Closes the serial communication port. </summary>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	CloseSerialPort(
	HANDLE							deviceHdl );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Closes a frame acquisition stream. </summary>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	CloseStream(
	HANDLE							deviceHdl );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Collects the offset values. </summary>
///
/// <remarks>
///		Collects the offset values for the offset value correction within the detector. Make sure
///		any X-Ray source is disabled and the high voltage is enabled and settled at the desired
///		value.
///	</remarks>
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
/// </param>
/// <param name="collectDcTimeOut">
///		[in]	Offset value collection time out (ms). Use returned value from ConfigureDetector()
///		or InitFwDefaults().
/// </param>
/// 
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	CollectOffsetValues(
	HANDLE							deviceHdl,
	ULONG							timeOut,
	ULONG							collectDcTimeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Configures the detector. </summary>
///
/// <remarks>
/// Configures the detector by applying the Hexitec sensor / state machine configuration, the
/// operation mode / data path configuration and the system parameters. If the resolution is
/// changed the acquisition pipeline has to be recreated by calling CreatePipeline(). The frame time
/// out for the acquisition is set to 2.5 times of the frame time (at least 25ms).
/// </remarks>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="sensorConfig">
/// 	[in]	Sets the Hexitec sensor / state machine configuration.
/// </param>
/// <param name="operationMode">
/// 	[in]	Sets the operation mode / data path configuration.
/// </param>
/// <param name="systemConfig">
/// 	[in]	Sets the system parameters.
/// </param>
/// <param name="width">
/// 	[out] 	Returns the width / X-Resolution of the frame based on the applied sensor
/// 	configuration. Use this value when calling SetFrameFormatControl().
/// </param>
/// <param name="height">
/// 	[out] 	Returns the height / Y-Resolution of the frame based on the applied sensor
/// 	configuration. Use this value when calling SetFrameFormatControl().
/// </param>
/// <param name="frameTime">
/// 	[out]	Returns the frame time (s) based on the applied detector configuration.
/// </param>
/// <param name="collectDcTime">
/// 	[out]	Returns the time (ms) which is needed to collect the offset values within the
/// 	detecor. Use this value when calling CollectOffsetValues().
/// </param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
/// </param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	ConfigureDetector(
	HANDLE							deviceHdl,
	CONST HexitecSensorConfigPtr	sensorConfig,
	CONST HexitecOperationModePtr	operationMode,
	CONST HexitecSystemConfigPtr	systemConfig,
	PUCHAR							width,
	PUCHAR							height,
	DOUBLE							*frameTime,
	PULONG							collectDcTime,
	ULONG							timeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Configures the detector with trigger options. </summary>
///
/// <remarks>
/// Configures the detector by applying the Hexitec sensor / state machine configuration, the
/// operation mode / data path configuration and the system parameters. If the resolution is
/// changed the acquisition pipeline has to be recreated by calling CreatePipeline(). The frame time
/// out for the acquisition is set to 2.5 times of the frame time (at least 25ms). The optional
/// trigger functionality (available since FW version 2) is set up additionally in comparison to
/// ConfigureDetector().
/// 
/// </remarks>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="sensorConfig">
/// 	[in]	Sets the Hexitec sensor / state machine configuration.
/// </param>
/// <param name="operationMode">
/// 	[in]	Sets the operation mode / data path configuration.
/// </param>
/// <param name="systemConfig">
/// 	[in]	Sets the system parameters.
/// </param>
/// <param name="width">
/// 	[out] 	Returns the width / X-Resolution of the frame based on the applied sensor
/// 	configuration. Use this value when calling SetFrameFormatControl().
/// </param>
/// <param name="height">
/// 	[out] 	Returns the height / Y-Resolution of the frame based on the applied sensor
/// 	configuration. Use this value when calling SetFrameFormatControl().
/// </param>
/// <param name="frameTime">
/// 	[out]	Returns the frame time (s) based on the applied detector configuration.
/// </param>
/// <param name="collectDcTime">
/// 	[out]	Returns the time (ms) which is needed to collect the offset values within the
/// 	detecor. Use this value when calling CollectOffsetValues().
/// </param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
/// </param>
/// <param name="enSyncMode">
///		[in]	Enables the triggered synchronous state machine start. The detector state machine is
///		started	and kept running with an applied high level on trigger input 1. With this
///		functionality a	synchronous start of multiple detectors can be achieved.
///		property | value
///		---------|--------
///		default	 | AS_CONTROL_DISABLED
/// </param>
/// <param name="enTriggerMode">
///		[in]	Enables the triggered data acquisition mode. The detector delivers n frames
///		(SetTriggeredFrameCount()) after a trigger event on trigger input 2 or frames as long as an
///		high level is applied on trigger input 3.
///		property | value
///		---------|--------
///		default	 | AS_CONTROL_DISABLED
/// </param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	ConfigureDetectorWithTrigger(
	HANDLE							deviceHdl,
	CONST HexitecSensorConfigPtr	sensorConfig,
	CONST HexitecOperationModePtr	operationMode,
	CONST HexitecSystemConfigPtr	systemConfig,
	PUCHAR							width,
	PUCHAR							height,
	DOUBLE							*frameTime,
	PULONG							collectDcTime,
	ULONG							timeOut,
	Control							enSyncMode,
	Control							enTriggerMode );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Create a frame acquisition pipeline. </summary>
///
/// <remarks>
///		Create a frame acquisition pipeline and the necessary internal buffer and transfer buffer.
///		The number of buffers and frames settings are tradeoff between memory usage and the
///		availability to achieve a losless data acquisition and data processing. Requires an opened
///		acuisition stream (OpenStream()). Has to be recalled if the resolution change.
/// </remarks>
/// 
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="bufferCount">
///		[in]	Number of internal frame acquisition buffers.
///		property         | value
///		-----------------|--------
///		default          | 512
///	</param>
/// <param name="transferBufferCount">
///		[in]	Number of transfer buffers.
///		property         | value
///		-----------------|--------
///		default          | 100
///	</param>
/// <param name="transferBufferFrameCount">
///		[in]	Number of frames within on transfer buffer.
///		property         | value
///		-----------------|--------
///		default          | 100
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	CreatePipeline(
	HANDLE							deviceHdl,
	ULONG							bufferCount,
	ULONG							transferBufferCount,
	ULONG							transferBufferFrameCount );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Closes a GigE device instance. </summary>
///
/// <remarks>
///		Disconnect from a GigE device and closes the GigE device instance.
/// </remarks>
/// 
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	ExitDevice(
	HANDLE							deviceHdl );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Returns an aSpect error code description. </summary>
/// 
/// <param name="asError">
///		[in]	An aSpect error code.
///	</param>
/// <param name="asErrorMsg">
///		[out]		Pointer to allocated memory. This memory is filled with the aSpect error
///		description.
///	</param>
/// <param name="length">
///		[in]	Size of the allocated memory.
///	</param>
///
/// <returns>	Returns 0 on success or an system error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	GetAsErrorMsg(
	LONG							asError,
	LPSTR							asErrorMsg,
	ULONG							length );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Get information strings from the connected GigE device. </summary>
/// 
/// <remarks>
///		The returned pointer to the strings are valid as long the device instance is valid.
/// </remarks>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="deviceInfoStr">
///		[out]	Pointer to the structure which contains the pointer to the strings.
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	GetDeviceInformation(
	HANDLE							deviceHdl,
	GigEDeviceInfoStrPtr			deviceInfoStr );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Gets the last occurred Pleora API error. </summary>
///
/// <remarks>
/// Gets the last occurred Pleora API error code, string and description. The string pointer can be
/// NULL. The memory will be filled partially if the length is not long enough to hold the entire
/// string. The necessary length to hold the complete string will be returned, as long the length
/// pointer is not NULL.
/// </remarks>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="pleoraErrorCode">
/// 	[out]	Returns a Pleora API result code.
/// </param>
/// <param name="pleoraErrorCodeString">
/// 	[out,opt]	Pointer to allocated memory. This memory is filled with the Pleora code string.
/// </param>
/// <param name="pleoraErrorCodeStringLen">
/// 	[in,out,opt] 	Size of the allocated memory [in] and necessary size to hold the complete
/// 	Pleora code string [out].
/// </param>
/// <param name="pleoraErrorDescription">
/// 	[out,opt]	Pointer to allocated memory. This memory is filled with the Pleora description
/// 	string.
/// </param>
/// <param name="pleoraErrorDescriptionLen">
/// 	[in,out,opt]	Size of the allocated memory [in] and necessary size to hold the complete
/// 	Pleora description string [out].
/// </param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	GetLastResult(
	HANDLE							deviceHdl,
	PULONG							pleoraErrorCode,
	LPSTR							pleoraErrorCodeString,
	PULONG							pleoraErrorCodeStringLen,
	LPSTR							pleoraErrorDescription,
	PULONG							pleoraErrorDescriptionLen );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Returns a system error code description. </summary>
///
/// <param name="sysError">
///		[in]	A system error code.
///	</param>
/// <param name="sysErrorMsg">
///		[out]		Pointer to allocated memory. This memory is filled with the system error
///		description.
///	</param>
/// <param name="length">
///		[in]	Size of the allocated memory.
///	</param>
///
/// <returns>	Returns 0 on success or an system error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	GetSystemErrorMsg(
	LONG							sysError,
	LPSTR							sysErrorMsg,
	ULONG							length );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Return the states of the trigger lines. </summary>
///
/// <remarks>
///		Return the states of the trigger lines.
/// </remarks>
/// 
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="trigger1">
///		[out]	State of trigger1.
///	</param>
/// <param name="trigger2">
///		[out]	State of trigger2.
///	</param>
/// <param name="trigger3">
///		[out]	State of trigger3.
///	</param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	GetTriggerState(
	HANDLE							deviceHdl,
	PUCHAR							trigger1,
	PUCHAR							trigger2,
	PUCHAR							trigger3,
	ULONG							timeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Initialize a GigE device instance. </summary>
///
/// <remarks>
/// Searches for an available GigE device. Creates a device instance and connect to the device. Also
/// returns the internal Peora API result and the related description. Refer to GetLastResult().
/// </remarks>
///
/// <param name="deviceHdl">
///		[out]	Returns a handle to a GigE device instance on success. Otherwise NULL.
///	</param>
/// <param name="deviceDescriptor">
/// 	[in,opt]	Pointer to a string which contains a device descriptor.
///		value            | behaviour
///		-----------------|--------
///		NULL or empty    | connect to the first available device
/// 	IP- or MAC-Addr. | connect to the specified device
/// </param>
/// <param name="pleoraErrorCode">
/// 	[out]	Returns a Pleora API result code.
/// </param>
/// <param name="pleoraErrorCodeString">
/// 	[out,opt]	Pointer to allocated memory. This memory is filled with the Pleora code string.
/// </param>
/// <param name="pleoraErrorCodeStringLen">
/// 	[in,out,opt] 	Size of the allocated memory [in] and necessary size to hold the complete
/// 	Pleora code string [out].
/// </param>
/// <param name="pleoraErrorDescription">
/// 	[out,opt]	Pointer to allocated memory. This memory is filled with the Pleora description
/// 	string.
/// </param>
/// <param name="pleoraErrorDescriptionLen">
/// 	[in,out,opt]	Size of the allocated memory [in] and necessary size to hold the complete
/// 	Pleora description string [out].
/// </param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	InitDevice(
	HANDLE							*deviceHdl,
	CONST LPSTR						deviceDescriptor,
	PULONG							pleoraErrorCode,
	LPSTR							pleoraErrorCodeString,
	PULONG							pleoraErrorCodeStringLen,
	LPSTR							pleoraErrorDescription,
	PULONG							pleoraErrorDescriptionLen );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Reinitializes a connected GigE device to its power on default settings. </summary>
///
/// <remarks>
/// Reinitializes / reconfigures a connected GigE device to its power on / firmware default
/// settings. The high voltage is set either to the power on default or the specified set point
/// during the offset value collection. After the offset collection the high voltage is set to 0 V
/// and have to be reset by calling SetDAC(). If the resolution is changed the acquisition pipeline
/// has to be recreated by calling CreatePipeline().
/// </remarks>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="setHv">
/// 	[in]	Selects the source for the high voltage set point.
///		value | behaviour
///		------|--------
///		0     | use the power on default high voltage set point
/// 	>0    | use the supplied high voltage set point
/// </param>
/// <param name="hvSetPoint">
///		[in,out]	Sets the high voltage.
///		property         | value
///		-----------------|--------
///		default          | 0 V todo
/// 	range            | -750..0 V
/// </param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
/// </param>
/// <param name="width">
/// 	[out] 	Returns the width / X-Resolution of the frame based on the applied sensor
/// 	configuration. Use this value when calling SetFrameFormatControl().
/// </param>
/// <param name="height">
/// 	[out] 	Returns the height / Y-Resolution of the frame based on the applied sensor
/// 	configuration. Use this value when calling SetFrameFormatControl().
/// </param>
/// <param name="sensorConfig">
/// 	[out]	Returns the applied Hexitec sensor / state machine configuration.
/// </param>
/// <param name="operationMode">
/// 	[out]	Returns the applied operation mode / data path configuration.
/// </param>
/// <param name="frameTime">
/// 	[out]	Returns the frame time (s) based on the applied detector configuration.
/// </param>
/// <param name="collectDcTime">
/// 	[out]	Returns the time (ms) which is needed to collect the offset values within the
/// 	detecor. Use this value when calling CollectOffsetValues().
/// </param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	InitFwDefaults(
	HANDLE							deviceHdl,
	UCHAR							setHv,
	DOUBLE							*hvSetPoint,
	ULONG							timeOut,
	PUCHAR							width,
	PUCHAR							height,
	HexitecSensorConfigPtr			sensorConfig,
	HexitecOperationModePtr			operationMode,
	DOUBLE							*frameTime,
	PULONG							collectDcTime );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Opens the serial communication port. </summary>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="serialPort">
///		[in]	Selects the serial communication port to use. Keep at 2.
///	</param>
/// <param name="rxBufferSize">
///		[in]	Sets the internal Pleora API receive buffer size.
///		property         | value
///		-----------------|--------
///		default          | 2048
///	</param>
/// <param name="useTermChar">
///		[in] 	Selects the usage of a termination character.
///		value | behaviour
///		------|--------
///		0     | termination character is not used
/// 	>0    | the specified termination character is used
///	</param>
/// <param name="termChar">
///    	Sets the used termination character. Keep at 0x0d.
/// </param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	OpenSerialPort(
	HANDLE							deviceHdl,
	LONG							serialPort,
	ULONG							rxBufferSize,
	UCHAR							useTermChar,
	UCHAR							termChar );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Opens a frame acquisition stream. </summary>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	OpenStream(
	HANDLE							deviceHdl );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Reads the environment values from the detector. </summary>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="humidity">
/// 	[out] The humidity within the detector chamber.
/// </param>
/// <param name="ambientTemperature">
/// 	[out] The ambient temperature within the detector chamber.
/// </param>
/// <param name="asicTemperature">
/// 	[out] The asic temperature.
/// </param>
/// <param name="adcTemperature">
///  	[out] The ADC temperature.
///  </param>
/// <param name="ntcTemperature">
///  	[out] The cooling control temperature.
///  </param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
/// </param>
///
/// <returns>	The environment values. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	ReadEnvironmentValues(
	HANDLE							deviceHdl,
	DOUBLE							*humidity,
	DOUBLE							*ambientTemperature,
	DOUBLE							*asicTemperature,
	DOUBLE							*adcTemperature,
	DOUBLE							*ntcTemperature,
	ULONG							timeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Reads the operating values from the detector. </summary>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="v3_3">			 	
///		[out] The voltage level for the 3.3V power rail.
///	</param>
/// <param name="hvMon">
/// 	[out] The voltage level for the high voltage monitor (no conversion gain applied).
/// </param>
/// <param name="hvOut">		 	
/// 	[out] The voltage level for the high voltage monitor.
/// </param>
/// <param name="v1_2">
///		[out] The voltage level for the 1.2V analogue power rail.
///	</param>
/// <param name="v1_8">
/// 	[out] The voltage level for the 1.8V power rail.
/// </param>
/// <param name="v3">
///		[out] The voltage level for the 3V power rail.
///	</param>
/// <param name="v2_5">
/// 	[out] The voltage level for the 2.5V power rail.
/// </param>
/// <param name="v3_3ln">
///  	[out] The voltage level for the 3.3V low noise power rail.
///  </param>
/// <param name="v1_65ln">
///  	[out] The voltage level for the 1.65V low noise power rail.
///  </param>
/// <param name="v1_8ana">
/// 	[out] The voltage level for the 1.8V analogue power rail.
/// </param>
/// <param name="v3_8ana">
/// 	[out] The voltage level for the 3.8V analogue power rail.
/// </param>
/// <param name="peltierCurrent">
/// 	[out] The current through the Peltier element.
/// </param>
/// <param name="ntcTemperature">
/// 	[out] The cooling control temperature.
/// </param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
/// </param>
///
/// <returns>	The operating values. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	ReadOperatingValues(
	HANDLE							deviceHdl,
	DOUBLE							*v3_3,
	DOUBLE							*hvMon,
	DOUBLE							*hvOut,
	DOUBLE							*v1_2,
	DOUBLE							*v1_8,
	DOUBLE							*v3,
	DOUBLE							*v2_5,
	DOUBLE							*v3_3ln,
	DOUBLE							*v1_65ln,
	DOUBLE							*v1_8ana,
	DOUBLE							*v3_8ana,
	DOUBLE							*peltierCurrent,
	DOUBLE							*ntcTemperature,
	ULONG							timeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Registers a callback function to signalize a filled transfer buffer. </summary>
///
/// <remarks>
///		Registers a callback function to signalize a third party application a filled transfer
///		buffer, which is ready for data processing. The function should transmit the pointer to the
///		application and should return in a short term to avoid skipping frames. Once the application
///		is ready working on the bufferthe pointer has to be returned to the pool by calling
///		ReturnBuffer().
/// </remarks>
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="transferBufferReadyCallBack">
///		[in]	Pointer to the callback function. which is called once a transfer buffer is filled and ready
///		for data processing.
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	RegisterTransferBufferReadyCallBack(
	HANDLE							deviceHdl,
	p_bufferCallBack				transferBufferReadyCallBack );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Returns a transfer buffer back to the available transfer buffer pool. </summary>
///
/// <remarks>
/// Returns a transfer buffer / pointer back to the available transfer buffer pool. The order
/// returning the pointers doesn't matter. If all buffer are handed over to the third party
/// application and no buffer is returned the acquisition function will return with a
/// 'No transfer buffer available' error.
/// </remarks>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="transferBuffer">
///		[in]	Pointer to the transfer buffer.
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	ReturnBuffer(
	HANDLE							deviceHdl,
	PUCHAR							transferBuffer );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Sends and / or receives bytes from the serial interface. </summary>
///
/// <remarks>
/// If sending bytes the receive buffer will be flushed before sending. The function will receive
/// a maximum number of bytes (rxBufferSize) within the specified time (timeOut). The function will
/// return a time out error if not enough bytes are received within the specified time. If the
/// interface is configured to use a termination char (OpenSerialPort()) the function will not
/// return a time out error if less bytes than specified are received and a the termination char is
/// received.
/// </remarks>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="txBuffer">
///		[in]	Buffer which holds the bytes to send.
///	</param>
/// <param name="txBufferSize">
///		[in]	Number of bytes to send.
///	</param>
/// <param name="bytesWritten">
///		[out]	Number of bytes sent.
///	</param>
/// <param name="rxBuffer">
///		[out]	Pointer to allocated memory. The received bytes will filled into this buffer.
///	</param>
/// <param name="rxBufferSize">
///		[in]	Size of the allocated memory.
///	</param>
/// <param name="bytesRead">
///		[out]	Number of bytes received.
///	</param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	SerialPortWriteRead(
	HANDLE							deviceHdl,
	CONST PUCHAR					txBuffer,
	ULONG							txBufferSize,
	PULONG							bytesWritten,
	PUCHAR							rxBuffer,
	ULONG							rxBufferSize,
	PULONG							bytesRead,
	ULONG							timeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Sets the DAC within the detector. </summary>
///
/// <remarks>
///		If a value is out of range it will coerced either to the minimal or to the maximal value.
///		The values will be overwritten by the function with the values set by the function
///		after coercing / digitization. Coercing will not return an error or warning.
///		
/// </remarks>
/// 
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="vCal">
///		[in,out]	Sets the calibration voltage.
///		property         | value
///		-----------------|--------
///		default          | 0.5 V
/// 	range            | 0..3 V
///	</param>
/// <param name="uMid">
///		[in,out]	Sets the buffer reference (mid) voltage.
///		property         | value
///		-----------------|--------
///		default          | 1 V
/// 	range            | 0..3 V
///	</param>
/// <param name="hvSetPoint">
///		[in,out]	Sets the high voltage.
///		property         | value
///		-----------------|--------
///		default          | 0 V todo
/// 	range            | -750..0 V
/// </param>
/// <param name="detCtrl">
///		[in,out]	Sets the DET voltage. Not used. Keep at default value.
///		property         | value
///		-----------------|--------
///		default          | 0 V
/// 	range            | 0..3 V
///	</param>
/// <param name="targetTemperature">
///		[in,out]	Sets the target temperature of the cooling system.
///		property         | value
///		-----------------|--------
///		default          | 20 degree C
/// 	range            | 10..40 degree C
///	</param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	SetDAC(
	HANDLE							deviceHdl,
	DOUBLE							*vCal,
	DOUBLE							*uMid,
	DOUBLE							*hvSetPoint,
	DOUBLE							*detCtrl,
	DOUBLE							*targetTemperature,
	ULONG							timeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Sets the application frame format. </summary>
///
/// <remarks>
/// Sets the application frame format to the Pleora GigE device (fixed for this application except
/// the resolution).
/// </remarks>
/// 
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="pixelFormat">
///		[in]	Sets the pixel format for the acquisition. Keep at "Mono14"
///	</param>
/// <param name="width">
///		[in]	Sets the acquisition width / X-Resolution.
///		property         | value
///		-----------------|--------
///		default          | use value from ConfigureDetector() or InitFwDefaults()
///		increment        | 4
/// 	range            | 4..16376-offsetX
///	</param>
/// <param name="height">
///		[in]	Sets the acquisition height / Y-Resolution.
///		property         | value
///		-----------------|--------
///		default          | use value from ConfigureDetector() or InitFwDefaults()
///		increment        | 1
/// 	range            | 1..16383-offsetY
///	</param>
/// <param name="offsetX">
///		[in]	Sets the acquisition offset in X-Direction. Keep at default value.
///		property         | value
///		-----------------|--------
///		default          | 0
///		increment        | 2
/// 	range            | 0..16376-width
///	</param>
/// <param name="offsetY">
///		[in]	Sets the acquisition offset in Y-Direction. Keep at default value.
///		property         | value
///		-----------------|--------
///		default          | 0
///		increment        | 1
/// 	range            | 0..16383-height
///	</param>
/// <param name="sensorTaps">
///		[in]	Sets the number of sensor taps. Keep at "One"
///	</param>
/// <param name="testPattern">
///		[in]	Enables the built in test pattern. Keep at "Off"
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	SetFrameFormatControl(
	HANDLE							deviceHdl,
	CONST LPSTR						pixelFormat,
	ULONGLONG						width,
	ULONGLONG						height,
	ULONGLONG						offsetX,
	ULONGLONG						offsetY,
	CONST LPSTR						sensorTaps,
	CONST LPSTR						testPattern );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Sets the frame time out for the acquisition. </summary>
///
/// <remarks>
///		Sets the frame time out for the second and the following frames within an acquisition. It
///		should be set to 2.5 times of the frame time. If this is less than 25ms this function set
///		time out to 25ms. In this case no error or warning will be returned. This function is
///		optional as the time out is set during ConfigureDetector() or ConfigureDetectorWithTrigger()
///		based on the frame time set up and only needed in special cases.
/// </remarks>
/// 
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="frameTimeOut">
///		[in]	Communication time out (ms).
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	SetFrameTimeOut(
	HANDLE							deviceHdl,
	ULONG							frameTimeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Set the number of frames acquired after a trigger event. </summary>
///
/// <remarks>
///		Set the number of frames acquired after a trigger event on trigger input 2
///		(EnableTriggerMode()).
/// </remarks>
/// 
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="frameCount">
///		[in]	Sets the number of frames to acquire.
///		property         | value
///		-----------------|--------
/// 	range            | 1..4294967295
///	</param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	SetTriggeredFrameCount(
	HANDLE							deviceHdl,
	ULONG							frameCount,
	ULONG							timeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Stops a running acquisition. </summary>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	StopAcquisition(
	HANDLE							deviceHdl );

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Uploads offset correction values. </summary>
///
/// <remarks>
/// Uploads the offset correction values to the hardware. The existing values will be replaced.
/// The values must not exceed 80x80 values and have to fit to the current operating conditions
/// (sensor configuration, DAC settings, high voltage, temperature, ...). The values have to be
/// arranged like [r0c0, r0c1, ... , r0c(width-1), r1c0, r1c1, ... , r(height-1)c(width-1)]
/// </remarks>
///
/// <param name="deviceHdl">
///		[in]	Handle to a valid GigE device instance.
///	</param>
/// <param name="offsetValues">
///		[in]	Array containing the offset values.
///	</param>
/// <param name="offsetvalueslength">
///		[in]	Length of the array.
///	</param>
/// <param name="timeOut">
///		[in]	Communication time out (ms).
///	</param>
///
/// <returns>	Returns 0 on success or an aSpect error code on failure. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C	GIGE_API	LONG	UploadOffsetValues(
	HANDLE							deviceHdl,
	PUSHORT							offsetValues,
	ULONG							offsetvalueslength,
	ULONG							timeOut );

////////////////////////////////////////////////////////////////////////////////////////////////////
// End of GigE.h
////////////////////////////////////////////////////////////////////////////////////////////////////