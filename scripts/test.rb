#! /usr/bin/env ruby

ENV['PKG_CONFIG_PATH'] = "#{File.expand_path(File.join("..", "build"), File.dirname(__FILE__))}:#{ENV['PKG_CONFIG_PATH']}"

pids = ARGV.map { |s| Integer(s) }

require 'orocos'
Orocos.initialize
Orocos.run 'taskmon_test' do |process|
    task = Orocos::TaskContext.get 'taskmon'
    # task.watch(process.pid)
    pids.each do |p|
        task.watch(p)
    end

    task.start
    r = task.stats.reader

    while true
        while s = r.read_new
            pp s
        end
        sleep 1
    end
end

