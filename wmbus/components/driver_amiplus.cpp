/*
 Copyright (C) 2019-2025 Fredrik Öhrström (gpl-3.0-or-later)
*/

#include"meters_common_implementation.h"

namespace
{
    struct Driver : public virtual MeterCommonImplementation
    {
        Driver(MeterInfo &mi, DriverInfo &di);
    };

    static bool ok = registerDriver([](DriverInfo&di)
    {
        di.setName("amiplus");
        di.setDefaultFields("name,id,total_energy_consumption_kwh,current_power_consumption_kw,total_energy_production_kwh,current_power_production_kw,voltage_at_phase_1_v,voltage_at_phase_2_v,voltage_at_phase_3_v,total_energy_consumption_tariff_1_kwh,total_energy_consumption_tariff_2_kwh,total_energy_consumption_tariff_3_kwh,total_energy_production_tariff_1_kwh,total_energy_production_tariff_2_kwh,total_energy_production_tariff_3_kwh,timestamp");
        di.setMeterType(MeterType::ElectricityMeter);
        di.addLinkMode(LinkMode::T1);
        //di.addDetection(MANUFACTURER_APA,  0x02,  0x02);
        //di.addDetection(MANUFACTURER_DEV,  0x37,  0x02);
        //di.addDetection(MANUFACTURER_DEV,  0x02,  0x00);
        //di.addDetection(MANUFACTURER_DEV,  0x02,  0x01);
        di.addDetection(0xae0c, 0x02, 0x01);  // KPL (Sanxing/Tauron)
        //di.addDetection(MANUFACTURER_APA,  0x02,  0x01);
        //di.addDetection(MANUFACTURER_APT,  0x02,  0x01);
        di.setConstructor([](MeterInfo& mi, DriverInfo& di){ return shared_ptr<Meter>(new Driver(mi, di)); });
    });

    Driver::Driver(MeterInfo &mi, DriverInfo &di) : MeterCommonImplementation(mi, di)
    {
        addNumericFieldWithExtractor(
            "total_energy_consumption",
            "The total energy consumption recorded by this meter.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::AnyEnergyVIF)
            );

        addNumericFieldWithExtractor(
            "current_power_consumption",
            "Current power consumption.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Power,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::PowerW)
            );

        addNumericFieldWithExtractor(
            "total_energy_production",
            "The total energy production recorded by this meter.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0E833C"))
            );

        addNumericFieldWithExtractor(
            "current_power_production",
            "Current power production.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Power,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BAB3C"))
            );

        // -- Voltage: VIFRange (standard Apator with 2F2F header) --
        addNumericFieldWithExtractor(
            "voltage_at_phase_1",
            "Voltage at phase L1.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::Voltage)
            .add(VIFCombinable::AtPhase1)
            );

        addNumericFieldWithExtractor(
            "voltage_at_phase_2",
            "Voltage at phase L2.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::Voltage)
            .add(VIFCombinable::AtPhase2)
            );

        addNumericFieldWithExtractor(
            "voltage_at_phase_3",
            "Voltage at phase L3.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::Voltage)
            .add(VIFCombinable::AtPhase3)
            );

        // -- Voltage: DifVifKey (KPL/Sanxing/Tauron with 60 9B COSEM header) --
        addNumericFieldWithExtractor(
            "voltage_at_phase_1",
            "Voltage at phase L1.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0AFDC9FC01"))
            );

        addNumericFieldWithExtractor(
            "voltage_at_phase_2",
            "Voltage at phase L2.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0AFDC9FC02"))
            );

        addNumericFieldWithExtractor(
            "voltage_at_phase_3",
            "Voltage at phase L3.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0AFDC9FC03"))
            );

        addStringFieldWithExtractor(
            "device_date_time",
            "Device date time.",
            DEFAULT_PRINT_PROPERTIES,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::DateTime)
            );

        addNumericFieldWithExtractor(
            "total_energy_consumption_tariff_1",
            "The total energy consumption recorded by this meter on tariff 1.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::AnyEnergyVIF)
            .set(TariffNr(1))
            );

        addNumericFieldWithExtractor(
            "total_energy_consumption_tariff_2",
            "The total energy consumption recorded by this meter on tariff 2.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::AnyEnergyVIF)
            .set(TariffNr(2))
            );

        addNumericFieldWithExtractor(
            "total_energy_consumption_tariff_3",
            "The total energy consumption recorded by this meter on tariff 3.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::AnyEnergyVIF)
            .set(TariffNr(3))
            );

        addNumericFieldWithExtractor(
            "total_energy_production_tariff_1",
            "The total energy production recorded by this meter on tariff 1.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E10833C"))
            );

        addNumericFieldWithExtractor(
            "total_energy_production_tariff_2",
            "The total energy production recorded by this meter on tariff 2.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E20833C"))
            );

        addNumericFieldWithExtractor(
            "total_energy_production_tariff_3",
            "The total energy production recorded by this meter on tariff 3.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E30833C"))
            );

        addNumericFieldWithExtractor(
            "max_power_consumption",
            "The maximum demand indicator.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Power,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Maximum)
            .set(VIFRange::AnyPowerVIF)
            );
    }
}

// Test: MyElectricity1 amiplus 10101010 NOKEY
// telegram=|4E4401061010101002027A00004005_2F2F0E035040691500000B2B300300066D00790C7423400C78371204860BABC8FC100000000E833C8074000000000BAB3C0000000AFDC9FC0136022F2F2F2F2F|
// {"_":"telegram","media":"electricity","meter":"amiplus","name":"MyElectricity1","id":"10101010","total_energy_consumption_kwh":15694.05,"current_power_consumption_kw":0.33,"total_energy_production_kwh":7.48,"current_power_production_kw":0,"voltage_at_phase_1_v":236,"device_date_time":"2019-03-20 12:57:00","timestamp":"1111-11-11T11:11:11Z"}
// |MyElectricity1;10101010;15694.05;0.33;7.48;0;236;null;null;null;null;null;null;null;null;1111-11-11 11:11.11